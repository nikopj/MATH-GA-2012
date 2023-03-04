#!/bin/bash

mkdir -p log.d

for opt in 0 3; do 
    g++ -Wall -O$opt MMult0.cpp 
    ./a.out > log.d/log$opt.txt
done
