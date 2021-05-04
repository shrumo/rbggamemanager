import subprocess
import os

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r,e]
    [e,e,e]
    [e,e,e])

#rules = ( ->red /*L*/. (
  up+
  down+
  left+
  right
)* /*R*/. /*B*/. [r] /*B*/. {r} /*L*/. (
  up+
  down+
  left+
  right
)* /*R*/. right /*L*/. (
  up+
  down
) /*R*/. ->blue )
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './remove_redundant_dots {} --state_to_erase left'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Simple case with left redundant dots removed:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules=="#rules=(->red(up+down+left+right)*.[r]{r}(up+down+left+right)*.right(up+down).->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r,e]
    [e,e,e]
    [e,e,e])

#rules = ->red . (up + down + left + right)* . . [r] . {r} . (up + down + left + right)* . right . (up + down) . ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './remove_redundant_dots {} --state_to_erase right'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Simple case with left redundant dots removed:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red.(up+down+left+right)*[r]{r}.(up+down+left+right)*right.(up+down)->blue)")
