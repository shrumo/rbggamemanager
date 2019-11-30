import subprocess
import datetime
import os

logging_filename = 'tmp_file_from_exceeds_deadline_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('Chess')
server_process = subprocess.Popen(['./start_server', '../rbgParser/examples/ticTacToe.rbg', '7982', '--deadline', '0.2', '--log', logging_filename], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '0.4'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '0.01'], stdout=subprocess.DEVNULL)

print('Server got client B. (Server:', server_process.stdout.readline(), ')')

deadline_exceeded = False
for line in server_process.stdout:
    line = str(line)
    if 'deadline exceeded' in line.lower() and 'client 0' in line.lower() and not deadline_exceeded:
        print('Client A exceeded the time limit as expected. (Server:',line,')')
        deadline_exceeded = True

assert(deadline_exceeded)

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)

assert(os.path.isfile(logging_filename))

with open(logging_filename) as f:
    print('The logging file exists.')
    elems = list(f.readline().split(' '))
    assert(len(elems) == 6)
    time, depth, available_moves, score_a, score_b, timeouts_text = elems
    assert(float(time) < 60) # A TicTacToe game over localhost shouldn't take long
    assert(0 < int(depth) <= 9)
    assert(0 <= int(score_a) <= 100)
    assert(0 <= int(score_b) <= 100)
    assert(1 <= int(available_moves) <= 45)
    assert('=' in timeouts_text)
    first_player_info, second_player_info = timeouts_text.split('=')[1].split(',')
    assert('(' in first_player_info)
    first_player_timeouts, first_player_first_move_timeouts = first_player_info.split('(')
    assert(0 < int(first_player_timeouts) <= 9)
    assert(int(first_player_first_move_timeouts[:-1]) == -1)

# print('Removing the logging file.')
# os.remove(logging_filename)

