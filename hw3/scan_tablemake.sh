#!/bin/bash

for i in {0..3}; do 
    echo "------------ 0$i ------------"
    g++ -std=c++11 -O$i -march=native -fopenmp omp-scan.cpp -o omp-scan

    for j in {1..8}; do 
        echo "NUM_THREADS=$j"
        OMP_NUM_THREADS=$j ./omp-scan
    done
done

