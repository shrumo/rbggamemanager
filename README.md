# rbggamemanager

This is a game manager that can be used to play games written in regular board games language as described in
[rbgParser](https://github.com/uicus/rbgParser).

### Information

The game manager supports almost all capabilities of the language that are supported by the parser.

### Building

To build the project cmake is essential in version at least 3.5.0. Also boost packages are needed to compile the project. 
On debian based distros one can get all required packages through apt repository like so:
```
sudo apt install g++
sudo apt install cmake
sudo apt install libboost-all-dev
```

In order to build the project one can try to run the script make.sh:
```
cd ./scripts
./make.sh
```
This script requires the code to be downloaded by git for example with git clone command:
```
git clone https://github.com/shrumo/rbggamemanager.git
```
This requires the git program which can also can be downloaded from apt with:
```
sudo apt install git
```

You can also open the file itself and learn from it.

### Benchmarks

To benchmark playing games one can use a benchmark provided in scripts folder. Benchmark should pull all needed files 
from git repositories for submodules and after compiling it it should execute for some examples that are provided in 
```
./examples/
```
In order to run the benchmark just run the benchmark.sh script in ./scripts directory:
```
cd ./scripts
./benchmark.sh
```

### TODO
* IMPORTANT: The arithmetic is now supported in parser the code needs changes
* IMPORTANT: Adapt to the greedy operators
* Change the board representation to graph as soon as it will be supported in parser
* Give nice interface to python programming
* Fix all todos
* Make asynchronous client
* Document code
* Make additional server communication protocol to get information as a client
