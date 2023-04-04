#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char** argv)
{
    int N = omp_get_max_threads();
    int n = 96;
    int i, j;

    int* x = (int*) malloc(n * N * sizeof(int));  // mark the location of a happening
    for(i=0; i<N*n; i++) x[i]=0;

    #pragma omp parallel
    {
        #pragma omp for schedule(static, 1)
        for (j = 0; j < n; j++) 
        {
            i = omp_get_thread_num();
            x[n*i + j] = 1;
        }
    }

    for(i=0; i<N; i++)
    {
        for(j=0; j<n; j++)
        {
            x[i*n + j] ? printf("*") : printf(" ");
        }
        printf("\n");
    }

    free(x);
    return 0;
}
