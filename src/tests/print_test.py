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

#rules = ->red up (left + right) up up (left + right {e}) up [b] ->blue
'''

with open(tmp_game_filename,'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput('./print {}'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup(left+right)upup(left+(right{e}))up[b]->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup(.left+.right)upup(.left+.(right{e}))up[b]->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --disable_adding_dots_in_shifttables true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup(left+right)upup(.left+.(right{e}))up[b]->blue)')

os.remove(tmp_game_filename)