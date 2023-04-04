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

## 3. OpenMP version of 2D Jacobi/Gauss-Seidel smoothing

## 4. Preview for final project summary on next assignment
