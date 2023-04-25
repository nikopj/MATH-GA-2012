# Homework 4

## 1. Green network test
We use the given [`pingpong.cpp`](https://github.com/NYU-HPC23/lecture7) and generate an executable via `mpic++` (see `makefile`). To run `pingpong`, we submit the following script via `sbatch pingpong.sbatch`:

```bash
#!/bin/bash 
#SBATCH --nodes=2            # requests 2 compute server
#SBATCH --ntasks-per-node=1  # runs 1 tasks on each server
#SBATCH --cpus-per-task=1    # uses 1 compute core per task
#SBATCH --time=10:00
#SBATCH --mem=2GB
#SBATCH --job-name=pingpong
#SBATCH --output=pingpong.out

# setup 
cd /scratch/npj226/MATH-GA-2012/hw4
module purge
module load intel/19.1.2 openmpi/intel/4.0.5 

# run
mpirun ./hello_world   # print processor names and ranks
mpirun ./pingpong 0 1  # run program
```

This script requets two CPUs on two different Greene nodes, sets up our environment to have the correct OpenMPI modules, and then runs our scritps. We first run the `hello_world` program to get a list of our process names and ranks. Then `pingpong 0 1` sends some communication between procecess 0 and 1 (which happens over the network) and prints benchmark statistics. The results are written in `pingping.out`, shown below,

```text
Hello world from processor cs471.hpc.nyu.edu, rank 1 out of 2 processors
Hello world from processor cs470.hpc.nyu.edu, rank 0 out of 2 processors
pingpong latency: 1.229200e-03 ms
pingpong bandwidth: 1.119834e+01 GB/s
```

## 2. MPI ring communication
### `int_ring`
The integer ring communication program is given in `int_ring.cpp`. The program works by 
having all processes first recieve the message from the rank below them (mod `world_size`), 
and then send this receieved message plus their own rank -- except `rank 0`. The `rank 0` process 
instead first sends its message then waits to receive from the last process. At the end of $N$ loops, 
we should get a final received message of `N * world_size * (world_size-1) / 2` (at `rank-0`). We verify this 
in the program (see that `error` is zero). We run `int_ring` over several repeats and average 
the run-time to estimate a communication latency as `N / time / world_size`. The following sbatch script 
launches our experiment,

```bash
# setup 
cd /scratch/npj226/MATH-GA-2012/hw4
module purge
module load intel/19.1.2 openmpi/intel/4.0.5 

# get processor names
mpirun ./hello_world   # print processor names and ranks

# run over num repeats (N)
printf "%10s %10s %10s\n" "N" "error" "latency"
for N in {5000..50000..5000}; do
    mpirun ./int_ring $N  # run program
done
```

and we get the following output in `int_ring.out`:

```bash
Hello world from processor cs379.hpc.nyu.edu, rank 0 out of 8 processors
Hello world from processor cs382.hpc.nyu.edu, rank 3 out of 8 processors
Hello world from processor cs380.hpc.nyu.edu, rank 1 out of 8 processors
Hello world from processor cs381.hpc.nyu.edu, rank 2 out of 8 processors
Hello world from processor cs383.hpc.nyu.edu, rank 4 out of 8 processors
Hello world from processor cs384.hpc.nyu.edu, rank 5 out of 8 processors
Hello world from processor cs385.hpc.nyu.edu, rank 6 out of 8 processors
Hello world from processor cs386.hpc.nyu.edu, rank 7 out of 8 processors
         N      error    latency
      5000          0 1.5835166250e-06
     10000          0 1.5459264750e-06
     15000          0 1.2259567250e-06
     20000          0 1.2904632688e-06
     25000          0 1.1948375700e-06
     30000          0 1.1729635000e-06
     35000          0 1.1375791143e-06
     40000          0 1.1399307719e-06
     45000          0 1.1048574861e-06
     50000          0 1.0879480200e-06
```

Note that our observed latency matches well with the `pingpong` program (about 1 micro second).
Strangely, our latency seems to be decreasing with the number of repeats, where 
we would expect the latency to remain relatively constant for large `N`.

### `2mb_ring`
We adapt the `int_ring` program to transmitting a 2M byte array in `2mb_ring.cpp`.
The resulting output is given below:
```bash
Hello world from processor cs025.hpc.nyu.edu, rank 0 out of 8 processors
Hello world from processor cs461.hpc.nyu.edu, rank 1 out of 8 processors
Hello world from processor cs467.hpc.nyu.edu, rank 7 out of 8 processors
Hello world from processor cs463.hpc.nyu.edu, rank 3 out of 8 processors
Hello world from processor cs462.hpc.nyu.edu, rank 2 out of 8 processors
Hello world from processor cs466.hpc.nyu.edu, rank 6 out of 8 processors
Hello world from processor cs464.hpc.nyu.edu, rank 4 out of 8 processors
Hello world from processor cs465.hpc.nyu.edu, rank 5 out of 8 processors
         N      error          latency
      5000          0 1.7708491088e-04
     10000          0 1.7640469590e-04
     15000          0 1.7612404618e-04
     20000          0 1.7605523876e-04
     25000          0 1.7665900846e-04
     30000          0 1.7589343606e-04
     35000          0 1.7587415839e-04
     40000          0 1.7592246624e-04
     45000          0 1.7578788659e-04
     50000          0 1.7583693970e-04
```
We see that the latency is constant over world-size, as expected.
This suggests that the discrepancy we say in `int_ring` was due to 
some overhead by MPI.


## 3. MPI `scan` (aka `cumsum`)
I've adatped the code in `../hw3/omp-scan.cpp` to `./mpi_scan.cpp` to 
perform the `cumsum` of a large vector over multiple processors.
The timing results over several processor numbers are given below:
```bash
Hello world from processor cs425.hpc.nyu.edu, rank 0 out of 8 processors
Hello world from processor cs428.hpc.nyu.edu, rank 3 out of 8 processors
Hello world from processor cs426.hpc.nyu.edu, rank 1 out of 8 processors
Hello world from processor cs431.hpc.nyu.edu, rank 6 out of 8 processors
Hello world from processor cs427.hpc.nyu.edu, rank 2 out of 8 processors
Hello world from processor cs432.hpc.nyu.edu, rank 7 out of 8 processors
Hello world from processor cs430.hpc.nyu.edu, rank 5 out of 8 processors
Hello world from processor cs429.hpc.nyu.edu, rank 4 out of 8 processors
 worldsize          N       time      error
         2    4032000   0.002917          0
         3    4032000   0.008775          0
         4    4032000   0.002310          0
         5    4032000   0.021756          0
         6    4032000   0.020220          0
         7    4032000   0.025833          0
         8    4032000   0.004814          0
```
The timings show sme strange behavior, perhaps to do with network communication.

## 4. Project 
The project is to implement the [Flash
Attention](https://github.com/HazyResearch/flash-attention) paper in both Julia
(me) and C (Lakshay) for both CPU and GPU. The method involves writing a tiling
matrix multiplication with a catch -- the matrix is first row-normalized.
Specifically,

$$ O = \mathrm{softmax}(QK^T)V $$

where $O, Q, K, V \in \mathbb{R}^{N \times d}$. Additionally there are a few
varients of this algorithm which involve imposing certain sparsity patterns on
$QK^T$ which make the operation more computationally managable with large $N$.
For example, my image processing research is concerned with this opeartion when
$\softmax(QK^T)$ has a [block circulant with circulant
blocks](https://stackoverflow.com/questions/17007587/diagonalization-of-block-circulant-matrix-with-circulant-blocks),
which arises from considering $O, Q, K, V$ as vectorized images and only
computing $q^Tk$ in local windows.

More details of our plans and progress can be found in our repository, [FlashAttention.jl](https://github.com/nikopj/FlashAttention.jl).

