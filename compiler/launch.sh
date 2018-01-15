#!/bin/bash

./compiler return_2.c
gcc -m32 return_2.s -o return_2
./return_2
echo $?

