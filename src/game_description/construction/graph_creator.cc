//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <utility/graph.h>
#include <utility/moves_visitor.h>

#include <memory>
#include <queue>

#include "game_description/construction/moves_creator.h"
#include "parser/parser_actions.h"
#include "parser/parser_wrapper.h"

namespace
{

  using namespace std;
  using namespace rbg;

  struct GraphCreatorResult
  {
    node_t initial;
    node_t final;
  };

  unique_ptr<Move> e_b() { return make_unique<EmptyBackward>(); }

  unique_ptr<Move> e_f() { return make_unique<EmptyForward>(); }

  Nfa<unique_ptr<Move>> ThompsonsConstruction(
      const rbg_parser::game_move &rbg_move, const Declarations &declarations);

  // GraphCreator is responsible for applying the Thompson construction to the
  // parsed abstract syntax tree. It creates the actions and gives them
  // appropriate numbers that come from the order they appeared in the game
  // description.
  class GraphCreator : public AstFunction<GraphCreatorResult>
  {
  public:
    explicit GraphCreator(Graph<unique_ptr<Move>> &graph,
                          const Declarations &declarations)
        : graph_(graph), declarations_(declarations) {}

    GraphCreatorResult SumCase(const rbg_parser::sum &sum) override
    {
      node_t initial = graph_.NewNode();
      node_t final = graph_.NewNode();
      for (const auto &child : sum.get_content())
      {
        GraphCreatorResult child_result =
            GraphCreator(graph_, declarations_)(*child);
        graph_.MergeWithNode(initial, child_result.initial);
        graph_.MergeWithNode(final, child_result.final);
      }
      return {initial, final};
    }

    GraphCreatorResult ConcatenationCase(
        const rbg_parser::concatenation &concatenation) override
    {
      node_t initial = graph_.NewNode();
      node_t last_final = initial;
      for (const auto &child : concatenation.get_content())
      {
        GraphCreatorResult child_result =
            GraphCreator(graph_, declarations_)(*child);
        graph_.MergeWithNode(last_final, child_result.initial);
        last_final = child_result.final;
      }
      return {initial, last_final};
    }

    GraphCreatorResult StarCase(const rbg_parser::star_move &move) override
    {
      node_t initial = graph_.NewNode();
      node_t final = graph_.NewNode();
      GraphCreatorResult child_result =
          GraphCreator(graph_, declarations_)(*move.get_content());
      graph_.AddEdge(initial, e_f(), child_result.initial);
      graph_.AddEdge(child_result.final, e_f(), final);
      graph_.AddEdge(child_result.final, e_b(), child_result.initial);
      graph_.AddEdge(initial, e_f(), final);
      return {initial, final};
    }

    GraphCreatorResult MoveCheckCase(
        const rbg_parser::move_check &move) override
    {
      auto check_move = make_unique<Condition>(
          std::make_unique<Nfa<unique_ptr<Move>>>(
              ThompsonsConstruction(*move.get_content(), declarations_)),
          move.is_negated(), move.index_in_expression());
      node_t initial = graph_.NewNode();
      node_t final = graph_.NewNode();
      graph_.AddEdge(initial, std::move(check_move), final);
      return {initial, final};
    }

    GraphCreatorResult GameMoveCase(const rbg_parser::game_move &m) override
    {
      // Move representation graph edge
      node_t initial = graph_.NewNode();
      node_t final = graph_.NewNode();
      graph_.AddEdge(initial, CreateMove(m, declarations_), final);
      return {initial, final};
    }

  private:
    Graph<unique_ptr<Move>> &graph_;
    const Declarations &declarations_;
  };

  // This returns an nfa that comes from the default Thompsons construction.
  Nfa<unique_ptr<Move>> ThompsonsConstruction(
      const rbg_parser::game_move &rbg_move, const Declarations &declarations)
  {
    Graph<unique_ptr<Move>> graph;
    GraphCreatorResult result = GraphCreator(graph, declarations)(rbg_move);
    return Nfa<unique_ptr<Move>>{std::move(graph), result.initial, result.final};
  }

  void AddVisitedChecks(Nfa<unique_ptr<Move>> *nfa)
  {
    queue<node_t> to_visit;
    unordered_set<node_t> visited;
    to_visit.push(nfa->initial);
    visited.insert(nfa->initial);

    while (!to_visit.empty())
    {
      node_t u = to_visit.front();
      to_visit.pop();
      for (edge_id_t transition_id : nfa->graph.edges_ids_from(u))
      {
        auto &transition = nfa->graph.GetEdge(transition_id);
        node_t v = transition.to();
        if (transition.content()->type() == MoveType::kConditionCheck)
        {
          AddVisitedChecks(
              &dynamic_cast<Condition *>(transition.modifiable_content().get())
                   ->nfa());
        }
        if (visited.find(transition.to()) == visited.end())
        {
          if (nfa->graph.edges_ids_to(transition.to()).size() > 1)
          {
            node_t visited_check = nfa->graph.NewNode();
            unordered_set<edge_id_t> transitions_to =
                nfa->graph.edges_ids_to(transition.to());
            for (edge_id_t in_transition_id : transitions_to)
            {
              nfa->graph.ChangeEdgeTarget(in_transition_id, visited_check);
            }
            nfa->graph.AddEdge(visited_check, make_unique<VisitedQuery>(), v);
            to_visit.push(v);
            visited.insert(v);
            visited.insert(visited_check);
          }
          else
          {
            to_visit.push(v);
            visited.insert(v);
          }
        }
      }
    }
  }

  void HandleMultipleOutNodes(Graph<unique_ptr<Move>> &graph)
  {
    for (node_t node :
         std::vector<node_t>(graph.nodes().begin(), graph.nodes().end()))
    {
      auto out_transitions = graph.edges_ids_from(node);
      if (out_transitions.size() > 1)
      {
        // Add epsilon edges if there are multiple edges from this node
        for (edge_id_t transition_id : out_transitions)
        {
          auto &transition = graph.GetEdge(transition_id);
          if (transition.content()->type() != MoveType::kBackwardEmpty &&
              transition.content()->type() != MoveType::kForwardEmpty)
          {
            node_t new_node = graph.NewNode();
            graph.AddEdge(node, e_b(), new_node);
            graph.ChangeEdgeSource(transition_id, new_node);
          }
        }

        // Erase duplicate epsilon edges leading to the same places
        std::unordered_set<node_t> handled;
        std::vector<edge_id_t> to_erase;
        for (edge_id_t transition_id : graph.edges_ids_from(node))
        {
          const auto &transition = graph.GetEdge(transition_id);
          if (handled.find(transition.to()) != handled.end())
          {
            to_erase.push_back(transition_id);
          }
          else
          {
            handled.insert(transition.to());
          }
        }
        for (edge_id_t transition_id : to_erase)
        {
          graph.EraseEdge(transition_id);
        }
      }
    }
    // Do the same for condition checks
    for (const auto &pair : graph.nodes_map())
    {
      node_t node = pair.first;
      auto out_transitions = graph.edges_ids_from(node);
      for (edge_id_t transition_id : out_transitions)
      {
        auto &transition = graph.GetEdge(transition_id);
        if (transition.content()->type() == MoveType::kConditionCheck)
        {
          HandleMultipleOutNodes(
              dynamic_cast<Condition *>(transition.modifiable_content().get())
                  ->nfa()
                  .graph);
        }
      }
    }
  }

  void EraseNoops(Nfa<unique_ptr<Move>> &nfa)
  {
    std::vector<edge_id_t> edges_without_effect = GetNoops(nfa);
    for (edge_id_t edge_id : edges_without_effect)
    {
      auto &transition =
          nfa.graph.GetEdge(edge_id);
      if (transition.from() == nfa.initial)
      {
        nfa.initial = transition.to();
      }
      if (transition.to() == nfa.final)
      {
        nfa.final = transition.to();
      }
      nfa.graph.MergeWithNode(transition.to(), transition.from());
      nfa.graph.EraseEdge(transition.id());
    }
  }
} // namespace

std::vector<edge_id_t> rbg::GetNoops(const Nfa<std::unique_ptr<Move>> &nfa)
{
  std::vector<edge_id_t> noops;
  for (const auto& edge : nfa.graph.edges_map())
  {
    edge_id_t edge_id = edge.first;
    const Edge<std::unique_ptr<Move>>& edge_content = edge.second;
    if (edge_content.content()->type() == MoveType::kNoop)
    {
      noops.push_back(edge_id);
    }
  }
  return noops;
}

NfaBoardProduct::NfaBoardProduct(const Nfa<std::unique_ptr<Move>> &nfa,
                                 const Board &board,
                                 const std::vector<vertex_id_t> &initials)
{
  queue<VertexNode> to_visit;
  for (vertex_id_t begin_vertex : initials)
  {
    to_visit.push({begin_vertex, nfa.initial});
    node_mapping_[{begin_vertex, nfa.initial}] = NewNode();
    reverse_node_mapping_[node_mapping_[{begin_vertex, nfa.initial}]] = {
        begin_vertex, nfa.initial};
  }

  std::unordered_map<node_t, std::unordered_set<vertex_id_t>>
      conditions_initials;
  std::unordered_map<node_t, const Nfa<std::unique_ptr<Move>> *>
      conditions_nfas;
  while (!to_visit.empty())
  {
    VertexNode top = to_visit.front();
    to_visit.pop();
    for (edge_id_t transition_id : nfa.graph.edges_ids_from(top.node))
    {

      
      const auto &transition = nfa.graph.GetEdge(transition_id);

      // Collect condition edges to create subgraphs of them later on
      if (transition.content()->type() == MoveType::kConditionCheck)
      {
        const auto &sub_nfa =
            dynamic_cast<const Condition *>(transition.content().get())->nfa();
        conditions_initials[sub_nfa.initial].insert(top.vertex);
        conditions_nfas[sub_nfa.initial] = &sub_nfa;
      }

      // Collect all vertices reachable using this action
      vector<vertex_id_t> reachable_vertices;
      if (transition.content()->type() == MoveType::kShiftType)
      {
        edge_id_t edge_id =
            dynamic_cast<Shift *>(transition.content().get())->edge_id();
        vertex_id_t next = board.NextVertex(top.vertex, edge_id);
        if (next != board.vertices_count())
        {
          reachable_vertices.push_back(next);
        }
      }
      else if (transition.content()->type() == MoveType::kShiftTableType)
      {
        const auto &table =
            dynamic_cast<ShiftTable *>(transition.content().get())->table();
        reachable_vertices.insert(reachable_vertices.end(),
                                  table[top.vertex].begin(),
                                  table[top.vertex].end());
      }
      else
      {
        reachable_vertices.push_back(top.vertex);
      }


      // Visit all unvisited reachable vertices nodes pairs
      for (vertex_id_t vertex : reachable_vertices)
      {
        VertexNode next_node_vertex_pair{vertex, transition.to()};
        if (node_mapping_.find(next_node_vertex_pair) == node_mapping_.end())
        {
          node_mapping_[next_node_vertex_pair] = NewNode();
          to_visit.push(next_node_vertex_pair);
          reverse_node_mapping_[node_mapping_[next_node_vertex_pair]] =
              next_node_vertex_pair;
        }
        node_t current_node = node_mapping_[top];
        node_t next_node = node_mapping_[next_node_vertex_pair];
        AddEdge(current_node, transition_id, next_node);
      }
    }
  }

  for (const auto &condition_initial : conditions_initials)
  {
    const auto &sub_nfa = *conditions_nfas.at(condition_initial.first);
    sub_graphs_[condition_initial.first] = make_unique<NfaBoardProduct>(
        sub_nfa, board,
        vector<vertex_id_t>(condition_initial.second.begin(),
                            condition_initial.second.end()));
  }
}

Nfa<std::unique_ptr<Move>> rbg::CreateNfa(const rbg_parser::game_move &rbg_move,
                                          const Declarations &declarations,
                                          bool optimize)
{
  auto nfa = ThompsonsConstruction(rbg_move, declarations);
  if (optimize)
  {
    EraseNoops(nfa);
    AddVisitedChecks(&nfa);
    HandleMultipleOutNodes(nfa.graph);
  }
  return nfa;
}
