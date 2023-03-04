#!/bin/bash

mkdir -p log.d

MAXIT=${1:-5000}
PRINT_SKIP=100
results="log.d/results.txt"

for opt in 0 3; do 
    g++ -Wall -O$opt laplace.cpp 

    for gs in 0 1; do 
        for N in 10 100 1000 100000; do 
            fn="log.d/log_gs${gs}_N${N}_O$opt.txt"

            echo "$opt, $gs, $N" >> $results
            ./a.out $N $MAXIT $PRINT_SKIP $gs > $fn

            tail $fn -n 2 >> $results
            echo ""       >> $results
        done
    done
done

