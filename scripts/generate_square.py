#!/usr/bin/python3

import argparse

parser = argparse.ArgumentParser(description='Program to generate square graph grid.')

parser.add_argument('width',type=int,help='width of the square board')
parser.add_argument('height',type=int,help='height of the square board')
parser.add_argument('-d',default=False,type=bool,help='if True then a square can have 8 neighbours, otherwise 4') 

args = parser.parse_args()

neighbours = [(x,y) for x in [-1,0,1] for y in [-1,0,1] if x != 0 or y != 0]
if not args.d:
  neighbours = [(x,y) for (x,y) in neighbours if abs(x) + abs(y) != 2]

# This will be used to format the negative values
def f(x):
  if x < 0:
    return 'n' + str(abs(x))
  else:
    return str(x)

for y in range(args.height):
  for x in range(args.width):
    print('v{}{} [empty]'.format(x,y),end=' ') 
    neighbours_texts = []
    for (dx,dy) in neighbours:
      x_p = x + dx
      y_p = y + dy
      if 0 <= x_p < args.width and 0 <= y_p < args.height:
        neighbours_texts += ['m{}{} : v{}{}'.format(f(dx),f(dy),x_p,y_p)]
    print('{',', '.join(neighbours_texts),'}')
    

