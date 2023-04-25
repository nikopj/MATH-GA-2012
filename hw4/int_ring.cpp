#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int NREPEAT = atoi(argv[1]);

    MPI_Status status;

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int msg_final = NREPEAT*(world_size-1)*world_size / 2;
    int msg_out;
    int msg_in = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    double tt = MPI_Wtime();

    for(int n = 0; n < NREPEAT; n++) {
        if(rank == 0) {
            msg_out = rank + msg_in;
            MPI_Send(&msg_out, 1, MPI_INT, (rank+1) % world_size, 999, MPI_COMM_WORLD);
            MPI_Recv(&msg_in,  1, MPI_INT, (rank-1) % world_size, 999, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(&msg_in,  1, MPI_INT, (rank-1) % world_size, 999, MPI_COMM_WORLD, &status);
            msg_out = rank + msg_in;
            MPI_Send(&msg_out, 1, MPI_INT, (rank+1) % world_size, 999, MPI_COMM_WORLD);
        }
    }
    tt = MPI_Wtime() - tt;

    if(rank == 0) {
        // N, error, average-latency
        printf("%10d %10d %1.10e\n", NREPEAT, msg_final - msg_in, tt / world_size / NREPEAT);
    }

    MPI_Finalize();
    return 0;
}
