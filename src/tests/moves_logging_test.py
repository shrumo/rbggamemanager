import subprocess
import datetime
import os
import time

moves_logging_filename = 'tmp_moves_file_from_client_disconnect_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('TicTacToe')
server_process = subprocess.Popen(['./start_server', '../rbgParser/examples/ticTacToe.rbg', '7981', '--log_moves', moves_logging_filename,'--limit','2'], stdout=subprocess.PIPE)

server_start_time = time.time()

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time', '10000.0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time', '10000.0'], stdout=subprocess.DEVNULL)

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

server_end_time = time.time()

assert(os.path.isfile(moves_logging_filename))

with open(moves_logging_filename) as f:
  print('The moves file exists.')
  lines = list(f.readlines())
  resets = 0
  for line in lines:
    if 'reset' in line:
        resets += 1
    else:
        elems = list(line.split())
        poses, mods = elems[::2], elems[1::2]
        for pose in poses:
          assert(1 <= int(pose) <= 9)
  assert(resets == 2)
print('Removing the logging file.')
os.remove(moves_logging_filename)
  


