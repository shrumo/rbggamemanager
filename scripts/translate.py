#!/usr/bin/python3
import re
import argparse

parser = argparse.ArgumentParser(description= \
'''This helps in the process of translations of old rbg games to new ones.

The board must be added and editied by hand. Script generate_squares.py is also
provided that is able to generate a square board of given size with typical
transitions.

Change all occurences of old shifts to new shifts.
This will change whole file such that (0,1) will become m01.
Also tries to change old (piece) to new {piece}.
This code also checks which shifts are needed and adds their definition at the begining of the file. 

Translation is not perfect. The bounds must be added by hand and sometimes weird constructions must be also fixed by hand.

The translated output is on stdout.
''')
parser.add_argument('file',type=str,help='path to file to change')
parser.add_argument('-d',type=bool,help='diagonals are neighbours in graph or not')

args = parser.parse_args()

result = ""

directions = {(x,y) for x in [-1,0,1] for y in [-1,0,1] if x != 0 or y != 0}
if not args.d:
  directions = {(x,y) for (x,y) in directions if abs(x) + abs(y) != 2}

base_directions = list(directions)

def text_number(number):
  return str(number) if number >= 0 else 'n' + str(abs(number))

def direction_edge(direction):
  x,y = direction
  return 'm'+text_number(x) + text_number(y) +' '

patterns_to_sub = []
for direction in directions:
  patterns_to_sub += [(re.compile(r'\(\s*'+str(direction[0])+r'\s*,\s*'+str(direction[1])+r'\s*\)'),direction_edge(direction))]


finding_pattern = re.compile(r'\(\s*([+-]?[0-9])\s*,\s*([+-]?[0-9])\s*\)')

additional_shifts = []

with open(args.file) as game:
  for line in game:
    for pattern,sub in patterns_to_sub:
      line = re.sub(pattern,sub,line)
    
    matches = finding_pattern.finditer(line)
    for match in matches:
      direction = int(match.groups()[0]), int(match.groups()[1])
      if direction not in directions:
        patterns_to_sub += [(re.compile('\(\s*'+str(direction[0])+'\s*,\s*'+str(direction[1])+'\s*\)'),direction_edge(direction))]
        directions |= {direction}
        line = patterns_to_sub[-1][0].sub(patterns_to_sub[-1][1],line)
        additional_shifts += [direction]
        
    result += line

if additional_shifts:
  min_additional_x = min(0,min(map(lambda x : x[0], additional_shifts)))
  min_additional_y = min(0,min(map(lambda x : x[1], additional_shifts)))
  max_additional_x = max(1,max(map(lambda x : x[0], additional_shifts)))
  max_additional_y = max(1,max(map(lambda x : x[1], additional_shifts)))

  import math
  from collections import deque

  optimal_shift = [[None for x in range(max_additional_x - min_additional_x + 1)] for y in range(max_additional_y - min_additional_y + 1)]
  optimal_shift[0][0] = []
  queue = deque()
  queue += [(0,0)]
  while queue:
    x,y = queue[0]
    queue.popleft()
    for dx, dy in base_directions:
      x_p = x + dx
      y_p = y + dy
      if min_additional_x <= x_p <= max_additional_x and \
         min_additional_y <= y_p <= max_additional_y:
        if optimal_shift[y_p][x_p] is None:
          optimal_shift[y_p][x_p] = [(dx,dy)] + optimal_shift[y][x] 
          queue += [(x_p,y_p)]

  for additional_shift in sorted(additional_shifts):
    text = '#' + direction_edge(additional_shift) + '= '
    for d in optimal_shift[additional_shift[1]][additional_shift[0]]:
      text += direction_edge(d) + ' '
    text += '\n'
    result = text + result

# This regex should not make anything illegal. It is safe but may not translate
# every single occurence. Problems are with macros applications.

piece_sub = re.compile(r'([\s\)\]\}])\(([a-zA-Z\~,]+)\)')
result, count = piece_sub.subn(r'\1{\2}',result)
while count > 0:
  result, count = piece_sub.subn(r'\1{\2}',result)
print(result)
