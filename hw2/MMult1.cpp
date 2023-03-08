// g++ -std=c++11 -O3 -march=native MMult1.cpp && ./a.out

#include <stdio.h>
#include <math.h>
#include <omp.h> 
#include "utils.h"

#define BLOCK_SIZE 16

// Note: matrices are stored in column major order; i.e. the array elements in
// the (m x n) matrix C are stored in the sequence: {C_00, C_10, ..., C_m0,
// C_01, C_11, ..., C_m1, C_02, ..., C_0n, C_1n, ..., C_mn}
double* MMult00(long m, long n, long k, double *a, double *b, double *c) {
  for (long j = 0; j < n; j++) {
    for (long p = 0; p < k; p++) {
      for (long i = 0; i < m; i++) {
        double A_ip = a[i+p*m];
        double B_pj = b[p+j*k];
        double C_ij = c[i+j*m];
        C_ij = C_ij + A_ip * B_pj;
        c[i+j*m] = C_ij;
      }
    }
  }
  return c;
}

double* MMult01(long m, long n, long k, double *a, double *b, double *c) {
  #pragma omp parallel for 
  for (long j = 0; j < n; j++) {
    for (long p = 0; p < k; p++) {
      for (long i = 0; i < m; i++) {
        double A_ip = a[i+p*m];
        double B_pj = b[p+j*k];
        double C_ij = c[i+j*m];
        C_ij = C_ij + A_ip * B_pj;
        c[i+j*m] = C_ij;
      }
    }
  }
  return c;
}

double* MMult10(long m, long n, long k, double *a, double *b, double *c) {
  for (long j = 0; j < n; j+=BLOCK_SIZE) {
    for (long p = 0; p < k; p+=BLOCK_SIZE) {
      for (long i = 0; i < m; i+=BLOCK_SIZE) {
        // block matmul
        for (long jj = j; jj < j+BLOCK_SIZE; jj++){
          for (long pp = p; pp < p+BLOCK_SIZE; pp++){
            for (long ii = i; ii < i+BLOCK_SIZE; ii++){
              double A_ip = a[ii+pp*m];
              double B_pj = b[pp+jj*k];
              double C_ij = c[ii+jj*m];
              C_ij = C_ij + A_ip * B_pj;
              c[ii+jj*m] = C_ij;
            }
          }
        }
      }
    }
  }
  return c;
}

double* MMult11(long m, long n, long k, double *a, double *b, double *c) {
  #pragma omp parallel for 
  for (long j = 0; j < n; j+=BLOCK_SIZE) {
    for (long p = 0; p < k; p+=BLOCK_SIZE) {
      for (long i = 0; i < m; i+=BLOCK_SIZE) {
        // block matmul
        for (long jj = j; jj < j+BLOCK_SIZE; jj++){
          for (long pp = p; pp < p+BLOCK_SIZE; pp++){
            for (long ii = i; ii < i+BLOCK_SIZE; ii++){
              double A_ip = a[ii+pp*m];
              double B_pj = b[pp+jj*k];
              double C_ij = c[ii+jj*m];
              C_ij = C_ij + A_ip * B_pj;
              c[ii+jj*m] = C_ij;
            }
          }
        }
      }
    }
  }
  return c;
}

double vec_max_error(long N, double *a, double *b)
{
    double max_err = 0;
    for (long i = 0; i < N; i++) max_err = fmax(max_err, fabs(a[i] - b[i]));
    return max_err;
}

int main(int argc, char** argv) {
  Timer tt;
  double t00, t01, t10, t11;
  const long PFIRST = BLOCK_SIZE;
  const long PLAST = 2000;
  const long PINC = std::max(50/BLOCK_SIZE,1) * BLOCK_SIZE; // multiple of BLOCK_SIZE

  srand48((long) time(NULL));

  printf("  Dim       F00      F01      F10       F11      BW00       BW01       BW10       BW11       Error\n");
  for (long p = PFIRST; p < PLAST; p += PINC) {
    long m = p, n = p, k = p;
    long NREPEATS = 1e9/(m*n*k)+1;
    double ops = NREPEATS * 2 * m * n * k / 1e9;
    double mems = NREPEATS * 4 * m * n * k * sizeof(double) / 1e9;

    double max_err = 0;
    double* a = (double*) aligned_malloc(m * k * sizeof(double)); // m x k
    double* b = (double*) aligned_malloc(k * n * sizeof(double)); // k x n
    double* c = (double*) aligned_malloc(m * n * sizeof(double)); // m x n
    double* c_ref = (double*) aligned_malloc(m * n * sizeof(double)); // m x n

    // Initialize matrices
    for (long i = 0; i < m*k; i++) a[i] = drand48() - 0.5;
    for (long i = 0; i < k*n; i++) b[i] = drand48() - 0.5;

    tt.tic();
    for (long rep = 0; rep < NREPEATS; rep++) { // Compute reference solution
      for(long i=0; i<m*n; i++) c_ref[i] = 0.0;
      c_ref = MMult00(m, n, k, a, b, c_ref);
    }
    t00 = tt.toc();

    tt.tic();
    for (long rep = 0; rep < NREPEATS; rep++) { 
      for(long i=0; i<m*n; i++) c[i] = 0.0;
      c = MMult01(m, n, k, a, b, c);
    }
    t01 = tt.toc();
    max_err = fmax(max_err, vec_max_error(m*n, c, c_ref));

    tt.tic();
    for (long rep = 0; rep < NREPEATS; rep++) { 
      for (long i = 0; i < m*n; i++) c[i] = 0.0;
      c = MMult10(m, n, k, a, b, c);
    }
    t10 = tt.toc();
    max_err = fmax(max_err, vec_max_error(m*n, c, c_ref));

    tt.tic();
    for (long rep = 0; rep < NREPEATS; rep++) { 
      for (long i = 0; i < m*n; i++) c[i] = 0.0;
      c = MMult11(m, n, k, a, b, c);
    }
    t11 = tt.toc();
    max_err = fmax(max_err, vec_max_error(m*n, c, c_ref));

    printf("%10ld %3.5f %5f %5f %5f %5f %5f %5f %5f %.3e\n", 
        p, ops/t00, ops/t01, ops/t10, ops/t11, 
        mems/t00, mems/t01, mems/t10, mems/t11, max_err);

    aligned_free(a);
    aligned_free(b);
    aligned_free(c);
    aligned_free(c_ref);
  }

  return 0;
}

// * Using MMult0 as a reference, implement MMult1 and try to rearrange loops to
// maximize performance. Measure performance for different loop arrangements and
// try to reason why you get the best performance for a particular order?
//
//
// * You will notice that the performance degrades for larger matrix sizes that
// do not fit in the cache. To improve the performance for larger matrices,
// implement a one level blocking scheme by using BLOCK_SIZE macro as the block
// size. By partitioning big matrices into smaller blocks that fit in the cache
// and multiplying these blocks together at a time, we can reduce the number of
// accesses to main memory. This resolves the main memory bandwidth bottleneck
// for large matrices and improves performance.
//
// NOTE: You can assume that the matrix dimensions are multiples of BLOCK_SIZE.
//
//
// * Experiment with different values for BLOCK_SIZE (use multiples of 4) and
// measure performance.  What is the optimal value for BLOCK_SIZE?
//
//
// * What percentage of the peak FLOP-rate do you achieve with your code?
//
//
// NOTE: Compile your code using the flag -march=native. This tells the compiler
// to generate the best output using the instruction set supported by your CPU
// architecture. Also, try using either of -O2 or -O3 optimization level flags.
// Be aware that -O2 can sometimes generate better output than using -O3 for
// programmer optimized code.
