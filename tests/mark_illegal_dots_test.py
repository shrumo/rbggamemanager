import subprocess
import os

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ->red (left + .left) ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
# assert('#rules' in out)
print('With illegal dots marked:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')