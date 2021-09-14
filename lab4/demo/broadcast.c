#include <stdio.h>
#include <mpi.h>

#define ROOT 0

int main (int argc, char **argv) {
    int rank, proc, a;
    
    // initializam procesele
    MPI_Init(&argc, &argv);
    
    // fiecare proces are un rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank
    // stabilim numarul de procese
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    // procesul root seteaza valoarea lui a
    if (rank == ROOT) {
        a = 69;
        printf("Value %d was broadcast by the process no. %d\n", a, rank);
    }

    // facem broadcast de la procesul zero pana la restul proceselor
    MPI_Bcast(&a, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    printf("Process no. %d got value %d\n", rank, a);

    MPI_Finalize();
    return 0;
}