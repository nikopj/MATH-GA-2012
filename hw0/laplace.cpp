#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "utils.h"

void printvec(double* v, long n1, long n2)
{
    for(long i=n1; i<n2; i++) printf("%f\n", v[i]);
}
void printvec(double* v, long N){ return printvec(v, 0, N); }

double* jacobi_iter(long N, double* u, double* u_old, double* f)
{
    double h2 = 1/pow(N+1, 2);

    // set previous value to current value
    for(long i=0; i<N+2; i++) u_old[i] = u[i]; 

    // boundary values u[0] = u[N+1] = 0
    for(long i=1; i<N+1; i++)
    {
        u[i] = 0.5 * (h2*f[i] + u_old[i-1] + u_old[i+1]);
    }
    return u;
}

double* gauss_seidel_iter(long N, double* u, double* f)
{
    double h2 = 1/pow(N+1, 2);

    // boundary values u[0] = u[N+1] = 0
    for(long i=1; i<N+1; i++)
    {
        u[i] = 0.5 * (h2*f[i] + u[i+1] + u[i-1]);
    }
    return u;
}

double normdiff(long N, double* u, double* v)
{
    double err = 0;
    for(long i=0; i<N; i++) err += pow(u[i] - v[i], 2);
    return sqrt(err);
}

double residual(long N, double* u, double *f)
{
    double h2 = 1/pow(N+1, 2);
    double res = 0;
    for(long i=1; i<N+1; i++){
        res += pow((2*u[i] - u[i-1] - u[i+1])/h2 - f[i], 2);
    }
    return sqrt(res);
}

int main(int argc, char** argv) {
    // argument parsing
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s [N] [MAXIT=5000] [PRINT_SKIP=1] [GAUSS_SEIDEL=0]\n", argv[0]);
        exit(-1);
    }

    errno = 0;
    const double TOL = 1e-4;
    const long N = strtol(argv[1], NULL, 10);

    long MAXIT = 5000;
    if(argc > 2) MAXIT = strtol(argv[2], NULL, 10);

    long PRINT_SKIP = 1;
    if(argc > 3) PRINT_SKIP = strtol(argv[3], NULL, 10);

    bool GAUSS_SEIDEL = false;
    if(argc > 4) GAUSS_SEIDEL = (strtod(argv[4], NULL) == 1) ? true : false;

    if(errno != 0)
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    printf("N=%ld, MAXIT=%ld, TOL=%f, PRINT_SKIP=%ld, GAUSS_SEIDEL=%s\n", \
            N, MAXIT, TOL, PRINT_SKIP, GAUSS_SEIDEL ? "true" : "false");

    double res, res0, x;
    double* f;     // data vector
    double* u;     // current solution vector
    double* u_old; // previous solution vector
    double* u_sol; // analytic solution vector
    
    // allocate and init
    f     = (double*) malloc((N+2) * sizeof(double));
    u_sol = (double*) malloc((N+2) * sizeof(double));
    u     = (double*) malloc((N+2) * sizeof(double));
    u_old = (double*) malloc((N+2) * sizeof(double));
    for(long i=0; i<N+2; i++)
    {
        f[i] = 1;
        u[i] = 0;
        x = (double) i / (N+1);
        u_sol[i] = 0.5 * x * (1 - x);
    }

    Timer t;
    t.tic();

    // solve
    res0 = residual(N, u, f); 
    printf("     k,      res,      solres\n");
    printf("%10d, %10f, %10f\n", 0, res0, normdiff(N, u_sol, u));
    for(long k=1; k<=MAXIT; k++)
    {
        GAUSS_SEIDEL ? gauss_seidel_iter(N, u, f) : jacobi_iter(N, u, u_old, f);
        res = residual(N, u, f);
        if(k % PRINT_SKIP == 0 || res < TOL*res0){
            printf("%10ld, %10f, %10f\n", k, residual(N, u, f), normdiff(N+2, u_sol, u));
        }
        if(res < TOL*res0) break;
    }
    printf("time = %f\n", t.toc());

    free(f);
    free(u);
    free(u_old);
    free(u_sol);
    return 0;
}
