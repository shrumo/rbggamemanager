#!/usr/bin/python3

# Script to create CMakeLists.txt for submodule

import os
import re

make = open('Makefile')

cxxflags = ''
cxxdebugflags = ''

for line in make:
    match = re.match('CXXFLAGS =(.*)',line)
    if match:
        cxxflags = match.group(1).strip()

    match = re.match('CXXFLAGSDEBUG =(.*)',line)
    if match:
        cxxdebugflags = match.group(1).strip()

make.close()

files = []
for filename in os.listdir('src'):
    if filename != 'main.cpp':
        files += ['src/' + filename]

text = '''
cmake_minimum_required(VERSION 3.5.0)
project(rbgParser)

set(CMAKE_CXXFLAGS "${{CMAKE_CXX_FLAGS}} {}")
set(CMAKE_CXX_FLAGS_DEBUG "${{CMAKE_CXX_FLAGS_DEBUG}} {}")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES {})
add_library(rbgParser SHARED ${{SOURCE_FILES}})
'''

print(text.format(cxxflags,cxxdebugflags,'\n'.join(files)))
