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

#rules = ->red . (up + down + left + right)* . . [r] . {r} . (up + down + left + right)* . right . (up + down) . ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_redundant_dots {}'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Simple case with redundant dots marked:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red/*L*/.(up+down+left+right)*/*R*/./*B*/.[r]/*B*/.{r}/*L*/.(up+down+left+right)*/*R*/.right/*L*/.(up+down)/*R*/.->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r,e]
    [e,e,e]
    [e,e,e])

#rules = ->red . (up + down + left + right)* [r] . {r} . (up + down + left + right)* . right . (up + down) . ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_redundant_dots {} --modifiers_as_dots false'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Simple case with redundant dots marked not treating modifiers as dots:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red/*L*/.(up+down+left+right)*[r]/*R*/.{r}/*L*/.(up+down+left+right)*/*R*/.right/*L*/.(up+down)/*R*/.->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r,e]
    [e,e,e]
    [e,e,e])

#anySquare = (left* + right*) (up* + down*)
#rules = ->> anySquare [e] (. left left* + . right right*) [e] ->>
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_redundant_dots {} --modifiers_as_dots false'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Tricky case, should not be redundant:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->>(left*+right*)(up*+down*)[e]((.leftleft*)+(.rightright*))[e]->>)")


tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r,e]
    [e,e,e]
    [e,e,e])

#anySquare = (left* + right*) (up* + down*)
#rules = ->> anySquare [e] (. left left + . right right) [e] ->>
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_redundant_dots {} --modifiers_as_dots false'.format(tmp_game_filename))
# assert('#rules' in out)
print(out)
print('Tricky case, should be right redundant:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->>(left*+right*)(up*+down*)[e]((/*R*/.leftleft)+(/*R*/.rightright))[e]->>)")

