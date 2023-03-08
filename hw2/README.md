# Homework 2

## Finding memory bugs using valgrind
### test01
`val_test01.cpp` computes the first 10 Fibonacci numbers. It contains two
memory errors,
1. the loop in `f` goes too far, accessing and writing to un-allocated memory.
2. `malloc`'d memory is freed using `delete`. This has undefined behavior, 
    and is fixed by instead using `free`.

### test02
`val_test02.cpp` fails to initialize array before doing computations with its
values. Notably, `memcheck` does not notice this unless an unititialized value
is printed, and as such an error is only thrown on line `104`. This program is
simply fixed by extending the initialization loop to touch all values.

Bugs in both programs were found and confirmed to be fixed using `valgrind
--tool=memcheck [programfile]`.

## Optimizing matrix-matrix multiplication
- comment on column major storage and nested-loop ordering
A naive implementation of matrix matrix mulitplication (MMM) by copying the MMM as inner-product formula $C_{ij} = \sum_k A_{ik}B_{kj}$,
```
for(i=0; i<m; i++)
    for(j=0; j<n; j++)
        for(k=0; k<m; k++)
            C[i,j] += A[i,k]*B[k,j];
```
For matrices stored in column-major format, this implementation does not take into account spatial locality well.
To do so, we want to decrease our frequency of accessing different columns (changes in `j`) so that we have more cache-hits when a cache line is copied over. Thus, it's best to place `j` as the outer-loop (2 col read per it), `k` as the middle (1 col read per it), and `i` as the inner loop as it naturally follows the memory storage order, i.e.,  
locality (and hence cache-hits)
- baseline
- tiling version BS=16, 32, 64, 128
- parallel version
- parallel + tiling?, BS=fastest one from tiling version alone

## Approximating Special Functions Using Taylor Series & Vectorization
done.

## Pipelining and Optimization
- one big plot of timings over dimension
done.

