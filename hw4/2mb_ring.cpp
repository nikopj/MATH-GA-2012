#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int NREPEAT = atoi(argv[1]);
    int NSIZE = 2000000; // send a 2Mb array

    MPI_Status status;

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char* msg_final  = (char*) malloc(NSIZE);
    for (int i = 0; i < NSIZE; i++) {
        msg_final[i] = 'a' + (i % 26);
    }

    char* msg_out  = (char*) malloc(NSIZE);
    memcpy(msg_out, msg_final, NSIZE);

    char* msg_in  = (char*) malloc(NSIZE);

    MPI_Barrier(MPI_COMM_WORLD);
    double tt = MPI_Wtime();

    for(int n = 0; n < NREPEAT; n++) {
        if(rank == 0) {
            MPI_Send(msg_out, NSIZE, MPI_CHAR, (rank+1) % world_size, 999, MPI_COMM_WORLD);
            MPI_Recv(msg_in, NSIZE, MPI_CHAR, world_size-1, 999, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(msg_in, NSIZE, MPI_CHAR, rank-1, 999, MPI_COMM_WORLD, &status);
            MPI_Send(msg_out, NSIZE, MPI_CHAR, (rank+1) % world_size, 999, MPI_COMM_WORLD);
        }
        memcpy(msg_out, msg_in, NSIZE);
    }
    tt = MPI_Wtime() - tt;

    int errors = 0;
    for (int i = 0; i < NSIZE; i++) {
        if (msg_in[i] != msg_final[i]) {
            errors++;
        }
    }

    if(rank == 0) {
        // N, error, average-latency
        printf("%10d %10d %1.10e\n", NREPEAT, errors, tt / world_size / NREPEAT);
    }

    free(msg_final);
    free(msg_out);
    free(msg_in);

    MPI_Finalize();
    return 0;
}
