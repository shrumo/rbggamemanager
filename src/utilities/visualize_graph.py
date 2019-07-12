#! /usr/bin/python3
import argparse
from collections import defaultdict
from tkinter import *

import rbg
from graph_widget import *


# Creates a Direct Acyclic Graph out of the nfa by erasing the backwards epsilon transitions.
# It also return the edges it erased.
def create_dag(nfa):
    adj = {v: set() for v in nfa.graph.nodes()}
    for edge in nfa.graph.edges().values():
        adj[edge.a()] |= {(edge.b(), edge.value())}

    erased = {v: [] for v in adj.keys()}
    entered = set()
    left = set()

    def dfs(u):
        nonlocal entered, left
        entered |= {u}
        for i, (v, value) in enumerate(list(adj[u])):
            if v not in entered:
                dfs(v)
            elif value == 'e' and v in entered and v not in left:
                adj[u] -= {(v, value)}
                erased[u] += [v]
        left |= {u}

    dfs(nfa.initial)
    return adj, erased


# Calculates the longest distance from the source to each point.
def dag_longest_distances(adj, source):
    result = defaultdict(lambda: 0)
    result[source] = 0
    no_in = []
    in_degrees = {v: 0 for v in adj.keys()}
    for u in adj.keys():
        for v, _ in adj[u]:
            in_degrees[v] += 1
    no_in += [source]
    while len(no_in) > 0:
        u = no_in[-1]
        no_in = no_in[:-1]
        for v, _ in adj[u]:
            in_degrees[v] -= 1
            result[v] = max(result[v], result[u] + 1)
            if in_degrees[v] == 0:
                no_in += [v]
    return result


# Calculates where the nfa nodes should be placed for the graph to look nice.
def nodes_positions(nfa):
    adj, erased = create_dag(nfa)
    distances = dag_longest_distances(adj, nfa.initial)
    erased_incoming = {v: [] for v in adj.keys()}
    for v in adj.keys():
        for u in erased[v]:
            erased_incoming[u] += [v]

    in_degrees = {v: 0 for v in adj.keys()}
    for u in adj.keys():
        for v, _ in adj[u]:
            in_degrees[v] += 1

    heights = {nfa.initial: 0}
    taken_heights = defaultdict(lambda: set())
    taken_heights[0] = {0}

    def dfs(u):
        nonlocal heights, taken_heights
        nexts = sorted([v for v, _ in adj[u]] + erased_incoming[u],
                       key=lambda x: (-distances[x], float('inf') if x not in heights else heights[x]))
        for i, v in enumerate(nexts):
            if v not in heights:
                heights[v] = heights[u]
                while any(heights[v] in taken_heights[i] for i in range(distances[u] + 1, distances[v] + 1)):
                    heights[v] += 1
                if heights[v] == heights[u]:
                    for i in range(distances[u] + 1, distances[v] + 1):
                        taken_heights[i] |= {heights[v]}
                else:
                    taken_heights[distances[v]] |= {heights[v]}
                dfs(v)

    dfs(nfa.initial)

    for v in nfa.graph.nodes():
        if v not in distances:
            distances[v] = -1
            heights[v] = 0
            while heights[v] in taken_heights[0]:
                heights[v] += 1
            taken_heights[0] |= {heights[v]}

    return {v: (distances[v], heights[v]) for v in nfa.graph.nodes()}


# Returns the graph tk object that visualizes the given game. The objects parent is set to root.
def create_nfa_visualization(root, game_text):
    graph = Graph(root)

    nfa = rbg.create_nfa(game_text)
    positions = nodes_positions(nfa)

    node_mapping = {}
    for node in nfa.graph.nodes():
        x, y = positions[node]
        node_mapping[node] = graph.add_node_rel(x * 400, -y * 400)

    def long_jump(a, b):
        dx = abs(a[0] - b[0])
        dy = abs(a[1] - b[1])
        return min(dx, dy) >= 2 or dy == 1 and dx > 20

    for edge in nfa.graph.edges().values():
        a = node_mapping[edge.a()]
        b = node_mapping[edge.b()]
        edge_label = edge.value()
        if len(edge_label) > 40:
            edge_label = '...'
        if long_jump(positions[edge.a()], positions[edge.b()]):
            graph.add_edge(a, b, edge_label, dash=(2, 8))
        else:
            graph.add_edge(a, b, edge_label)

    return graph


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("game_path", help="The path to the rbg game that should be visualized.")
    args = parser.parse_args()

    root = Tk()

    root.title(args.game_path)

    root.geometry("800x600")

    Grid.rowconfigure(root, 0, weight=1)
    Grid.columnconfigure(root, 0, weight=1)

    graph = create_nfa_visualization(root, open(args.game_path).read())
    graph.grid(column=0, row=0, sticky=N + S + E + W)

    root.mainloop()


if __name__ == '__main__':
    main()
