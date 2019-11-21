import subprocess
import datetime
import os
import time

logging_filename = 'tmp_file_from_continous_play_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('TicTacToe')
server_process = subprocess.Popen(['./start_server', '../rbgParser/examples/ticTacToe.rbg', '7981', '--log', logging_filename, '--limit', '100'], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time', '60000.0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time', '60000.0'], stdout=subprocess.DEVNULL)

print('Server got client B. (Server:', server_process.stdout.readline(), ')')

moves_done = 0
for line in server_process.stdout:
    line = str(line)
    print(line)

a_client_process.wait(timeout=30)
b_client_process.wait(timeout=30)
server_process.wait(timeout=30)

a_client_process.poll()
b_client_process.poll()
server_process.poll()

assert(os.path.isfile(logging_filename))

with open(logging_filename) as f:
  print('The logging file exists.')
  lines = list(f.readlines())
  assert(len(lines) == 100)
  for line in lines:
    elems = list(line.split(' '))
    assert(len(elems) == 5)
    time, depth, available_moves, score_a, score_b = elems
    assert(float(time) < 60) # A TicTacToe game over localhost shouldn't take long
    assert(0 < int(depth) <= 9)
    assert(0 <= int(score_a) <= 100)
    assert(0 <= int(score_b) <= 100)
    assert(1 <= int(available_moves) <= 45)

print('Removing the logging file.')
os.remove(logging_filename)
  


