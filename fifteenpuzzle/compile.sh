#!/bin/bash

clang++-5.0 -std=c++17 -O3 -I /home/daniele/libs/boost_1_66_0/install/x86/include/ -L /home/daniele/libs/boost_1_66_0/install/x86/lib/ -lboost_system -lboost_chrono fifteen.cpp -o fifteen
