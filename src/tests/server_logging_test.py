import subprocess
import datetime
import os

logging_filename = 'tmp_file_from_logging_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('TicTacToe')
server_process = subprocess.Popen(['./start_server', '../rbgParser/examples/ticTacToe.rbg', '7981', '--log', logging_filename], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0'], stdout=subprocess.DEVNULL)

print('Server got client B. (Server:', server_process.stdout.readline(), ')')

moves_done = 0
for line in server_process.stdout:
    line = str(line)
    if 'Got a move' in line:
        moves_done+=1
        print('Server is working, moves done:', moves_done)

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)

assert(os.path.isfile(logging_filename))

with open(logging_filename) as f:
  print('The logging file exists.')
  elems = list(f.readline().split(','))
  assert(len(elems) == 4)
  time, depth, score_a, score_b = elems
  assert(float(time) < 60) # A TicTacToe game over localhost shouldn't take long
  assert(0 < int(depth) <= 9)
  assert(0 <= int(score_a) <= 100)
  assert(0 <= int(score_b) <= 100)

print('Removing the logging file.')
os.remove(logging_filename)
  

