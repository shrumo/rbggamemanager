import subprocess

print('Chess')
server_process = subprocess.Popen(['./start_server', '../extern/rbgGames/games/chess.rbg', '7981'], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server waiting for the second client. (Server:', server_process.stdout.readline(), ')')
print('Server got client B. (Server:', server_process.stdout.readline(), ')')

for line in server_process.stdout:
    line = str(line)
    print(line)
    if 'turnCount' in line:
        print('Server is working, turn:', line.split(':')[1][:-3])

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)

print('TicTacToe')
server_process = subprocess.Popen(['./start_server', '../extern/rbgGames/games/ticTacToe.rbg', '7981'], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7981', '--seed', '0', '--time','0'], stdout=subprocess.DEVNULL)

print('Server waiting for the second client. (Server:', server_process.stdout.readline(), ')')
print('Server got client B. (Server:', server_process.stdout.readline(), ')')

moves_done = 0
for line in server_process.stdout:
    line = str(line)
    print(line)

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

assert(a_client_process.poll() == 0)
assert(b_client_process.poll() == 0)
assert(server_process.poll() == 0)
