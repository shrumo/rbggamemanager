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

#rules = ->red up* (left + right) (up {e})* up (left + right {e}) up [b] (left* + right*) (up* + down*) ->blue
'''

with open(tmp_game_filename,'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput('./print {} --modifier_indices true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('With modifier_indices:',rules)
assert(rules == '#rules=(/*1*/->redup*(left+right)(up{e})*up(left+(right{e}))up/*2*/[b](left*+right*)(up*+down*)/*3*/->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('With add_dots_in_alternatives:',rules)
assert(rules == '#rules=(->redup*(.left+.right)(up{e})*up(.left+.(right{e}))up[b](.left*+.right*)(.up*+.down*)->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --disable_adding_dots_in_shifttables true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('With add_dots_in_alternatives and disable_adding_dots_in_shifttables:',rules)
assert(rules == '#rules=(->redup*(left+right)(up{e})*up(.left+.(right{e}))up[b](left*+right*)(up*+down*)->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_stars true --add_dots_after_stars true --add_dots_after_alternatives true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('With add_dots_in_stars and add_dots_after_stars and add_dots_after_alternatives:',rules)
assert(rules == '#rules=(->red(.up)*(left+right).(.(up{e}))*.up(left+(right{e})).up[b]((.left)*+(.right)*)((.up)*+(.down)*).->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --add_dots_in_stars true --add_dots_after_stars true --add_dots_after_alternatives true --disable_adding_dots_in_shifttables true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('Some dots options with disable_adding_dots_in_shifttables:',rules)
assert(rules == '#rules=(->redup*(left+right).(.(up{e}))*.up(.left+.(right{e})).up[b](left*+right*)(up*+down*).->blue)')


out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --disable_adding_dots_in_shifttables true --one_dot_or_modifier_per_concat true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('Dots in alternatives, omitting shifttables and omitting same concatenation dots:',rules)
assert(rules == '#rules=(->redup*(left+right)(up{e})*up(.left+.(right{e}))up[b](left*+right*)(up*+down*)->blue)')


out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --add_dots_after_alternatives true --one_dot_or_modifier_per_concat true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('Dots in and after alternatives omitting same concatenation dots:',rules)
assert(rules == '#rules=(->redup*(.left+.right)(up{e})*up(.left+.(right{e}))up[b](.left*+.right*)(.up*+.down*)->blue)')

os.remove(tmp_game_filename)

second_tmp_game_filename = 'second_tmp_game.rbg'
second_tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, x, y
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [y,x]
    [e,e])

#rules = ([x] + [y])
'''

with open(second_tmp_game_filename,'w') as f:
    print(second_tmp_game_text, file=f)

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --one_dot_or_modifier_per_concat true'.format(second_tmp_game_filename))

rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
print('([x] + [y]) with dots in alternative, when omitting dots on the same level as modifier:',rules)
assert(rules == '#rules=([x]+[y])')

os.remove(second_tmp_game_filename)

