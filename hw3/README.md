# Homework 3

**gcc**: version 12.2.0
**Hardware**: same as homework2.


## 1. OpenMP warm-up
We consider the following block of code executed with two threads, where the execution 
of `f(i)` takes `i` milliseconds.

```c
#pragma omp parallel
{
    #pragma omp for schedule(static)
    for (i = 1; i < n; i++) f(i)
    #pragma omp for schedule(static)
    for (i = 1; i < n; i++) f(n-i)
}
```

Lets futher assume `n-1` is divisible by two, for simplicity.
Let `T0` and `T1` refer to the first and second thread, respectively. 

- (a) The static scheduler will divide the for loop into two continuous chunks,
  one for each thread. The first chunk of a single loop takes,
  $$t_0 = \sum_{i=1}^{(n-1)/2-1} i = (n-1)(n-3)/8 ~ \mathrm{ms},$$
  
  and the second chunk takes,
  $$t_1= \sum_{i=(n-1)/2}^{n-1} i = 3(n-1)(n+1)/8 ~ \mathrm{ms}.$$
  
  Clearly, $t_0 < t_1$. Let
  $\Delta t = t_1-t_0 = (n+3)(n-1)/4$. In the first loop, `T0` takes $t_1$ ms and then waits
  $\Delta t$ ms for `T1` to finish. In the second loop, `T0` and `T1` switch
  positions: `T1` takes $t_0$ ms and then waits $\Delta t$ ms for `T0` to
  finish. In  total, the parallel block takes $2t_1$ ms, of which 
  $2\Delta t= (n+3)(n-1)/2$ ms is spent waiting. In the limit of large $n$, the program
  spends two-thirds of its time wating.

- (b) `schedule(static, 1)` tells openmp to split the for loops between threads
  in an alternating fashion. The result is a change in our formulas for $t_0$
  and $t_1$,
  
  $$t_0 = \sum_{i=0}^{(n-1)/2}(2i+1) - n  = (n-1)^2 / 4 ~ \mathrm{ms},$$
  
  $$t_1 = \sum_{i=1}^{(n-1)/2} 2i = (n-1)(n+1) / 4 ~ \mathrm{ms}.$$
  
  Hence, $\Delta t = (n-1)/2$. The parallel bock still takes $2t_1$ ms with
  $2\Delta t$ ms spent waiting. However now as $n \rightarrow \infty$, the
  amount of time spent waiting approaches zero!

- (c) `schedule(dynamic, 1)` gives each thread a single loop-index at a time,
  waiting for a thread to finish the current index before giving it another. On
  a "perfect" machine -- one with no competing workloads -- this scheduling
  will perform exactly the same as `schedule(static, 1)` (for the case of f(i+1) taking longer than f(i)).

- (d) We can eliminate waiting time completely by using the directive `#pragma omp for nowait`.
  In this case, each thread continues on from the first for loop as soon as it has finished its part.
  Both threads will finish simultaneously in $t_0 + t_1 = n(n-1)/2$ ms.

## 2. Parallel Scan in OpenMP
In this problem, we implement the `scan` function, a.k.a. a cumulative sum function (see `scan-omp.cpp`).
I rewrote the serial code to be a bit more clear to myself (it also only uses 2 reads+write per iter instead of 3):

```c
void cumsum(long *result, long *input, long length) 
{
    long sum = 0;
    for (long i = 0; i < length; i++) 
    {
        sum += input[i];
        result[i] = sum;
    }
}
```

To multi-thread this function over `max_threads` threads, we can split the array into 
`max_threads` contiguous chunks and over each chunk perform a `cumsum`. After, 
we want to correct the results of each chunk by the last value in each chunk that came 
before it. This is implemented in the following function, `cumsum_omp`:

```c
void cumsum_omp(long *result, const long *input, long length) 
{
    long max_threads = omp_get_max_threads();
    long *local_sums = (long*) malloc(max_threads * sizeof(long));
    long chunk_size = length / max_threads;

    #pragma omp parallel
    {
        long thread_num = omp_get_thread_num();
        long sum = 0;

        long start_index = thread_num * chunk_size;
        long end_index = (thread_num == max_threads - 1) ? length : (start_index + chunk_size);

        for (long i = start_index; i < end_index; i++) 
        {
            sum += input[i];
            result[i] = sum;
        }

        local_sums[thread_num] = sum;
    }

    for (long i = 1; i < max_threads; i++) 
    {
        local_sums[i] += local_sums[i-1];
    }

    #pragma omp parallel
    {
        long thread_num = omp_get_thread_num();

        long start_index = thread_num * chunk_size;
        long end_index = (thread_num == max_threads - 1) ? length : (start_index + chunk_size);

        if (thread_num > 0) 
        {
            long correction = local_sums[thread_num - 1];
            for (long i = start_index; i < end_index; i++) 
            {
                result[i] += correction;
            }
        }
    }

    free(local_sums);
}
```

Note that we use two `parallel` regions, as in between we want the threads to
sync/stop so that we can perform a cumsum over `local_sums` to obtain the
proper correction factors. Out of curiousity, a second multi-threaded cumsum,
`cumsum_omp2`, is given the the program that uses a single `parallel` 
region. This is done by using the `barrier` and `single` pragma directives.

Below we show the timing results for `cumsum` vs. `cumsum_omp` for different
number of threads and compilaiton optimiation levels (O0, O1, O2, O3). The
program was run on a 4-core, 8-thread intel machine (see hw2 for details).

| Data        | serial| t=1   | t=2   | t=3   | t=4   | t=5   | t=6   | t=7   | t=8   |
|-------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
| O0-time (s) | 0.059 | 0.023 | 0.026 | 0.018 | 0.017 | 0.016 | 0.016 | 0.015 | 0.031 |
| O1-time (s) | 0.040 | 0.009 | 0.013 | 0.013 | 0.014 | 0.014 | 0.014 | 0.014 | 0.015 |
| O2-time (s) | 0.040 | 0.010 | 0.013 | 0.013 | 0.014 | 0.014 | 0.014 | 0.014 | 0.014 |
| O3-time (s) | 0.040 | 0.009 | 0.012 | 0.013 | 0.013 | 0.014 | 0.014 | 0.014 | 0.014 |

We see that without compilation optimization, increasing the number of threads
imporves the speed of the program (except at `threads=8`). However, once we
turn on compilation optimization, multithreading seems to no longer have a
positive effect. What's also interesting is that at no point is `threads=1`
timing equal to the serial timing. I have found no explanation for this
phenomenon.

For reference, the above table is achieved by the script `hw3/scan_tablemake.sh`,

## 3. OpenMP version of 2D Jacobi/Gauss-Seidel smoothing
in progress...

## 4. Preview for final project summary on next assignment
I am working with Lakshay Garg to implement the
[flash-attention](https://github.com/HazyResearch/flash-attention) paper in
Julia/C++. The paper describes a method for implementing the following
operation (dot-product attention), common in transformer neural networks,

$$\mathrm{softmax}(QK^T)V$$

for $Q , K, V \in R^{N\times d}$, where $\mathrm{softmax}$ is the row-wise 
normalization,

$$\mathrm{sm}\left(X\right)_{ij}\ =\frac{\exp\left(X_{ij}\ \right)}{\sum_{_{j=1}}^N\exp\left(X_{ij}\right)}$$

The catch is to implement the above operation without every forming the matrix $QK^T$, and can be 
done with matrix tiling and keeping track of some statistics. Furthermore, there are varients 
on this operation where we impose a sparsity pattern on the matrix $QK^T$.

The goal is to implement a few different varients of flash-attention (fa):
- dense fa
- block-sparse fa (1D, 2D)
- sliding-window sparse fa (1D, 2D)

Julia also makes it (relatively) easy to write CUDA kernels for running your
code on NVIDIA GPUs. 

Lakshay and I will try and write 
these different flash-attention functions in parallel -- I'll write Julia and Lakshay C++. 
We can then compare implementations for correctness and benchmark them against each other 
and the naive version of dot-product attention. 

