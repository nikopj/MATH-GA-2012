#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num() { return 0;}
inline omp_int_t omp_get_num_threads() { return 1;}
inline omp_int_t omp_get_max_threads() { return 1;}
#endif

#define MAXIT 50000
#define TOL 1e-8

void gs(double* u, double* f, long N, long* k, long maxit, double tol) {
    double diff;
    double h2 = 1.0 / pow(N-1, 2);
    for (*k = 0; *k < maxit; *k += 1) {
        diff = 0.0;
        #pragma omp parallel for reduction(max:diff)
        for (int n = N+1; n < N*(N-1) - 1; n += 2) {
            long i =  n / N;
            long j =  n % N;
            if (i > 0 && j > 0 && i < N-1 && j < N-1) {
                double u_new = 0.25 * (h2*f[n] + u[n-N] + u[n-1] + u[n+N] + u[n+1]);
                diff = fmax(diff, fabs(u_new - u[n]));
                u[n] = u_new;
            }
        }
        #pragma omp parallel for reduction(max:diff)
        for (int n = N+2; n < N*(N-1) - 1; n += 2) {
            long i =  n / N;
            long j =  n % N;
            if (i > 0 && j > 0 && i < N-1 && j < N-1) {
                double u_new = 0.25 * (h2*f[n] + u[n-N] + u[n-1] + u[n+N] + u[n+1]);
                diff = fmax(diff, fabs(u_new - u[n]));
                u[n] = u_new;
            }
        }
        if (diff < tol) {
            break;
        }
    }
}

void print2d(double* u, long N) {
    for (long n=0; n<N*N; n++) {
        long j =  n % N;
        printf("%1.3f ", u[n]);
        if (j == (N-1)) printf("\n");
    }
}

int main() {
    double *u, *f;
    double tt;
    long N, k;
    int num_threads = omp_get_max_threads();

    printf("%10s %10s %10s %10s\n", "nthreads", "npoints", "niters", "time");

    for (N = 20; N <= 240; N += 20) {
        u = (double*) malloc(N * N * sizeof(double));
        f = (double*) malloc(N * N * sizeof(double));
        for (long n = 0; n < N*N; n++) {
            u[n] = 0.0;
            f[n] = 1.0;
        }

        #if defined(_OPENMP)
        double start_time = omp_get_wtime();
        #else
        clock_t start_time = clock();
        #endif

        gs(u, f, N, &k, MAXIT, TOL);

        #if defined(_OPENMP)
        tt = omp_get_wtime() - start_time;
        #else
        tt = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        #endif

        // print2d(u, N);

        printf("%10d %10d %10d %10.4f\n", num_threads, N, k, tt);
        free(u);
        free(f);
    }
    return 0;
}
