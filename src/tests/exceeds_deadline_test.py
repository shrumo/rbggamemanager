import subprocess

print('Chess')
server_process = subprocess.Popen(['./start_server', '../rbgParser/examples/chess.rbg', '7982', '--deadline', '0.2'], stdout=subprocess.PIPE)

print('Server waiting for clients. (Server:', server_process.stdout.readline(), ')')

a_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '0.4'], stdout=subprocess.DEVNULL)

print('Server got client A. (Server:', server_process.stdout.readline(), ')')

b_client_process = subprocess.Popen(['./start_random_client', 'localhost', '7982','--seed', '0', '--sleep', '0.1'], stdout=subprocess.DEVNULL)

print('Server got client B. (Server:', server_process.stdout.readline(), ')')

deadline_exceeded = False
for line in server_process.stdout:
    line = str(line)
    print(line)
    if 'deadline exceeded' in line.lower() and 'client 0' in line.lower():
        print('Client A exceeded the time limit as expected. (Server:',line,')')
        deadline_exceeded = True

assert(deadline_exceeded)

server_process.kill()
a_client_process.kill()
b_client_process.kill()

a_client_process.wait(timeout=10)
b_client_process.wait(timeout=10)
server_process.wait(timeout=10)

print('Clients throw errors as expected.')
