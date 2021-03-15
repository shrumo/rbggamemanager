import subprocess
import datetime
import os

results_logging_filename = 'tmp_file_from_manager_exceeds_deadline_test_' + datetime.datetime.now().strftime("%I_%M%p_%B_%d_%Y")

print('Chess')
server_process = subprocess.Popen(['./start_server', '../extern/rbgGames/games/chess.rbg', '7982', '--deadline', '0.01', '--log_results', results_logging_filename], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server waiting for the second client. (Server:', server_process.stdout.readline(), ')')
print('Server got client B. (Server:', server_process.stdout.readline(), ')')

deadline_exceeded = False
for line in server_process.stdout:
    line = str(line)
    if 'manager exceeded' in line.lower() and not deadline_exceeded:
        print('Manager exceeded the deadline as expected')
        deadline_exceeded = True

assert(deadline_exceeded)

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)

assert(os.path.isfile(results_logging_filename))

with open(results_logging_filename) as f:
    print('The logging file exists.')
    elems = list(f.readline().split())
    assert(len(elems) == 6)
    _, depth, _, score_a, score_b, timeouts_text = elems
    assert(0 <= int(score_a) <= 100)
    assert(0 <= int(score_b) <= 100)
    assert('=' in timeouts_text)
    first_player_info, second_player_info, manager = timeouts_text.split('=')[1].split(',')
    assert('manager' in manager)

print('Removing the logging file.')
os.remove(results_logging_filename)

