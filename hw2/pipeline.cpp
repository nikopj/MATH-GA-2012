// Following V. Eijkhout Section 1.7.3
#include <stdio.h>
#include <math.h>
#include <omp.h> 
#include <time.h>
#include "utils.h"

double inp_ref(long N, double *a, double *b)
{
    double s = 0;
    for(long i=0; i<N; i++) s += a[i]*b[i];
    return s;
}

double inp_u2_0(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0;
    long i = 0;
    while(i < N)
    {
        sum1 += a[i] * b[i];
        sum2 += a[i+1] * b[i+1];
        i += 2;
    }
    return sum1 + sum2;
}

double inp_u4_0(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    long i = 0;
    while(i < N)
    {
        sum1 += a[i] * b[i];
        sum2 += a[i+1] * b[i+1];
        sum3 += a[i+2] * b[i+2];
        sum4 += a[i+3] * b[i+3];
        i += 4;
    }
    return sum1 + sum2 + sum3 + sum4;
}

double inp_u2_1(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0;
    double temp1 = 0, temp2 = 0;
    long i = 0;
    while(i < N)
    {
        temp1 = a[i] * b[i];
        temp2 = a[i+1] * b[i+1];

        sum1 += temp1; 
        sum2 += temp2;

        i += 2;
    }
    return sum1 + sum2;
}

double inp_u4_1(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    double temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
    long i = 0;
    while(i < N)
    {
        temp1 = a[i] * b[i];
        temp2 = a[i+1] * b[i+1];
        temp3 = a[i+2] * b[i+2];
        temp4 = a[i+3] * b[i+3];

        sum1 += temp1; 
        sum2 += temp2;
        sum3 += temp3;
        sum4 += temp4;

        i += 4;
    }
    return sum1 + sum2 + sum3 + sum4;
}

double inp_u2_2(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0;
    double temp1 = 0, temp2 = 0;
    long i = 0;
    while(i < N)
    {
        sum1 += temp1; 
        temp1 = a[i] * b[i];

        sum2 += temp2;
        temp2 = a[i+1] * b[i+1];

        i += 2;
    }
    return sum1 + sum2 + temp1 + temp2;
}

double inp_u4_2(long N, double *a, double *b)
{
    double sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    double temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
    long i = 0;
    while(i < N)
    {
        sum1 += temp1; 
        temp1 = a[i] * b[i];

        sum2 += temp2;
        temp2 = a[i+1] * b[i+1];

        sum3 += temp3;
        temp3 = a[i+2] * b[i+2];

        sum4 += temp4;
        temp4 = a[i+3] * b[i+3];

        i += 4;
    }
    return sum1 + sum2 + sum3 + sum4 + temp1 + temp2 + temp3 + temp4;
}

int main(int argc, char** argv) 
{
    Timer t;
    long NREPEATS = 500;
    long NINC = 32768;
    long NFIRST = 4096;
    long NLAST = NINC*40 + NFIRST;

    srand48((long) time(NULL));

    printf("     N      ref      u2_0      u2_1     u2_2     u4_0      u4_1      u4_2      error\n");
    for(long N=NFIRST; N <= NLAST; N += NINC)
    {
        double* a = (double*) aligned_malloc(N * sizeof(double)); 
        double* b = (double*) aligned_malloc(N * sizeof(double)); 
        double s, s_ref, err, maxerr = 0;

        for(long i=0; i<N; i++) 
        { 
            a[i] = drand48(); 
            b[i] = drand48(); 
        }

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s_ref = inp_ref(N, a, b);
        double t_ref = t.toc();

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u2_0(N, a, b);
        double t_u20 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u2_1(N, a, b);
        double t_u21 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u2_2(N, a, b);
        double t_u22 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u4_0(N, a, b);
        double t_u40 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u4_1(N, a, b);
        double t_u41 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;

        t.tic();
        for(long rep=0; rep < NREPEATS; rep++) s = inp_u4_2(N, a, b);
        double t_u42 = t.toc();
        err = fabs(s_ref - s);
        maxerr = err > maxerr ? err : maxerr;


        printf("%10ld %5f %5f %5f %5f %5f %5f %5f %.3e\n", N, t_ref, t_u20, t_u21, t_u22, t_u40, t_u41, t_u42, maxerr);
        
        aligned_free(a);
        aligned_free(b);
    }

    return 0;
}

