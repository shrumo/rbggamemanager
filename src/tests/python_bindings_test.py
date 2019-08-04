#! /usr/bin/python3
import rbg

nfa = rbg.create_nfa("""
#players = red(100), blue(100)
#pieces = e
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,e]
    [e,e])
//       1       2    3      4      5        6                 7
#rules = ->red ( up + down + left + right )* [$ turn = turn+1] ->blue
""")


def collect_actions(nfa):
    return [edge.value for edge in nfa.graph.edges().values()]

out_of_initial = [edge for edge in nfa.graph.edges().values() if edge.a() == nfa.initial]
assert (len(out_of_initial) == 1)
assert (out_of_initial[0].value() == "->red")

print('Edges of the nfa:')
print(*sorted(nfa.graph.edges().values(), key=lambda x: (x.a(), x.b(), x.value())), sep='\n')
