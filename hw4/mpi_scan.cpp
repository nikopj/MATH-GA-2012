#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void cumsum_mpi(long *result, const long *input, long length, int rank, int size, MPI_Comm comm)
{
    long *local_sums = (long*) malloc(size * sizeof(long));

    long sum = 0;
    for (long i = 0; i < length; i++)
    {
        sum += input[i];
        result[i] = sum;
    }

    MPI_Allgather(&sum, 1, MPI_LONG, local_sums, 1, MPI_LONG, comm);

    for (long i = 1; i < size; i++)
    {
        local_sums[i] += local_sums[i-1];
    }

    if (rank > 0)
    {
        long correction = local_sums[rank - 1];
        for (long i = 0; i < length; i++)
        {
            result[i] += correction;
        }
    }
    free(local_sums);
}

int main(int argc, char** argv)
{
    int rank, size;
    long N = 2*3*4*5*6*7*8*100;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) printf("%10s %10s %10s %10s\n", "worldsize", "N", "time", "error");

    long chunk_size = N / size;

    // data vector A, solution vector C
    long *A = (long*) malloc(N * sizeof(long));
    long *C = (long*) malloc(N * sizeof(long));

    for (long i = 0; i < N; i++) A[i] = rand(); 

    // compute solution in C
    long sum = 0;
    for (long i = 0; i < N; i++)
    {
        sum += A[i];
        C[i] = sum;
    }

    // initialize result array
    long *B0 = (long*) malloc(chunk_size * sizeof(long));
    for (long i = 0; i < chunk_size; i++) B0[i] = 0;

    long *B1 = (long*) malloc(chunk_size * sizeof(long)); 
    long *C0 = (long*) malloc(chunk_size * sizeof(long));

    // scatter data to B1
    MPI_Scatter(A, chunk_size, MPI_LONG, B1, chunk_size, MPI_LONG, 0, MPI_COMM_WORLD);

    // scatter solution to C0
    MPI_Scatter(C, chunk_size, MPI_LONG, C0, chunk_size, MPI_LONG, 0, MPI_COMM_WORLD);

    // compute chunk solution in B0
    double tt = MPI_Wtime();
    cumsum_mpi(B0, B1, chunk_size, rank, size, MPI_COMM_WORLD);
    tt = MPI_Wtime() - tt;

    long err = 0;
    for (long i = 0; i < chunk_size; i++)
    {
        err = std::max(err, std::abs(C0[i] - B0[i]));
    }

    long max_err;
    MPI_Reduce(&err, &max_err, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("%10d %10d %10f %10ld\n", size, N, tt, max_err);
    }
    MPI_Finalize();
    return 0;
}

