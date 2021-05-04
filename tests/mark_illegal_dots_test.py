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

#rules = (right + .right) ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Simple case with illegal dots marked:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=((right+(/*illegal*/.right))->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = (.left)* ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('No illegal dots here:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=((.left)*->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = (.left* + .right*) (.up* + .down*) ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Dots in typical anySquare:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(((/*illegal*/.left*)+(/*illegal*/.right*))((/*illegal*/.up*)+(/*illegal*/.down*))->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = (right + {r}) (.right)* ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots understand paths:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=((right+{r})(/*illegal*/.right)*->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = (. + {r}) (.right)* ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots understand paths:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=((/*illegal*/.+{r})(.right)*->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ([r] right + . right) ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('No illegal dots here because of modifier in one path:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=((([r]right)+(.right))->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ->red (right + . right) [b] (down + . down) [r] (.left* + .right*) (.up* + .down*) [e] (.right)*->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots marked even with modifiers separating them:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules ==
       "#rules=(->red(right+(/*illegal*/.right))[b](down+(/*illegal*/.down))[r]((/*illegal*/.left*)+(/*illegal*/.right*))((/*illegal*/.up*)+(/*illegal*/.down*))[e](.right)*->blue)")



tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = 
    rx0y0[b]{cycle:rx1y0}
    rx1y0[r]{cycle:rx0y0}
#rules = ->red (.cycle)*->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots marked in weird cyclic boards')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red(/*illegal*/.cycle)*->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ->red (([r] + {r,e}) . )* ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots marked in cycles:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red(([r]+{e,r})/*illegal*/.)*->blue)")

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ->red (([r] + [e]) . )* ->blue
'''

with open(tmp_game_filename, 'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput(
    './mark_illegal_dots {}'.format(tmp_game_filename))
assert('#rules' in out)
print('Illegal dots not marked in cycles if there are modifiers:')
print(out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ', '').replace('\n', '')
assert(rules == "#rules=(->red(([r]+[e]).)*->blue)")

