#!/bin/bash

for i in {1..8}; do 
    OMP_NUM_THREADS=$i ./jacobi2D-omp >> logs/jacobi.txt
    OMP_NUM_THREADS=$i ./gs2D-omp >> logs/gs.txt
done
