#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <omp.h>

void cumsum(long *result, long *input, long length) 
{
    long sum = 0;
    for (long i = 0; i < length; i++) 
    {
        sum += input[i];
        result[i] = sum;
    }
}

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

void cumsum_omp2(long *result, const long *input, long length) 
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

        #pragma omp barrier

        #pragma omp single
        {
            for (long i = 1; i < max_threads; i++) 
            {
                local_sums[i] += local_sums[i-1];
            }
        }

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

int main() {
    long N = 10000000;
    long* A = (long*) malloc(N * sizeof(long));
    long* B0 = (long*) malloc(N * sizeof(long));
    long* B1 = (long*) malloc(N * sizeof(long));
    for (long i = 0; i < N; i++) A[i] = rand();
    for (long i = 0; i < N; i++) B1[i] = 0;

    double tt = omp_get_wtime();
    cumsum(B0, A, N);
    printf("sequential-scan = %fs\n", omp_get_wtime() - tt);

    tt = omp_get_wtime();
    cumsum_omp(B1, A, N);
    printf("parallel-scan   = %fs\n", omp_get_wtime() - tt);

    long err = 0;
    for (long i = 0; i < N; i++) err = std::max(err, std::abs(B0[i] - B1[i]));
    printf("error = %ld\n", err);

    free(A);
    free(B0);
    free(B1);
    return 0;
}
