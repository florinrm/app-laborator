#include <stdio.h>
#include <mpi.h>

#define SENDER 0
#define RECEIVER 1

int main (int argc, char **argv) {
    // rank = id-ul procesului
    // proc = numarul de procese
    int rank, proc, a;
    
    // initializam procesele
    MPI_Init(&argc, &argv);
    
    // fiecare proces are un rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank
    // stabilim numarul de procese
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    // send si receive
    // un proces ii trimite informatia altui proces
    if (rank == SENDER) {
        a = 69;
        MPI_Send(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Value %d was sent by the process no. %d\n", a, rank);
    }

    if (rank == RECEIVER) {
        MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf ("Process no. %d got value %d\n\n", rank, a);
    }

    MPI_Finalize();
    return 0;
}