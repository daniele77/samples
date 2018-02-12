#!/bin/bash

export LD_LIBRARY_PATH=/home/daniele/libs/boost_1_66_0/install/x86/lib
../dcc return_2.c
gcc -m32 return_2.s -o return_2
./return_2
echo $?

