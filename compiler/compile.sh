#!/bin/bash

EXE=dcc

# CXX=g++
CXX=clang++-5.0

${CXX} -std=c++1y -Wall -O3 main.cpp -o $EXE -isystem /home/daniele/libs/boost_1_66_0/install/x86/include -L /home/daniele/libs/boost_1_66_0/install/x86/lib -lboost_filesystem -lboost_system
# ${CXX} -std=c++1y -Wall -O3 spirit_grammar.cpp -o $EXE -isystem /home/daniele/libs/boost_1_66_0/install/x86/include -L /home/daniele/libs/boost_1_66_0/install/x86/lib -lboost_filesystem -lboost_system
