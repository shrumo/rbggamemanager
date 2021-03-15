import subprocess
import datetime
import os

results_logging_filename = 'tmp_file_player_long_preparation_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('ticTacToe')
server_process = subprocess.Popen(['./start_server', '../extern/rbgGames/games/ticTacToe.rbg', '7982', '--deadline', '100', '--log_results', results_logging_filename, '--time_for_player_preparation', '400'], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '80', '--additional_preparation_time', '600', '--time','0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '80', '--additional_preparation_time','200', '--time','0'], stdout=subprocess.DEVNULL)

print('Server waiting for the second client. (Server:', server_process.stdout.readline(), ')')
print('Server got client B. (Server:', server_process.stdout.readline(), ')')

for line in server_process.stdout:
    line = str(line)
    if 'turnCount' in line:
        print('Server is working, turn:', line.split(':')[1][:-3])

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)

assert(os.path.isfile(results_logging_filename))

assert(os.path.isfile(results_logging_filename))

with open(results_logging_filename) as f:
  print('The logging file exists. Checking whether there was a preparation timeout.')
  elems = list(f.readline().split(' '))
  print(elems)
  assert(len(elems) == 6)
  time, depth, available_moves, score_a, score_b, timeout = elems
  assert(float(time) < 60) # A TicTacToe game over localhost shouldn't take long
  assert(0 < int(depth) <= 9)
  assert(0 <= int(score_a) <= 100)
  assert(0 <= int(score_b) <= 100)
  assert(1 <= int(available_moves) <= 45)
  assert('lateready' in timeout)

print('Removing the logging file.')
os.remove(results_logging_filename)
