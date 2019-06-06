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

# def collect_actions(nfa):
# return [edge.content for edge in nfa.graph.edges().values()]


# assert (len(set(collect_actions(nfa))) == 7 + 1)

# print("The nfa was succesfully created:", nfa)
