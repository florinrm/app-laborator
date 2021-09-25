#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MASTER 0
#define CHUNK 50

int calculate_sum(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

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

    int size = CHUNK * proc;
    int proc_array_size = size / proc;
    int *proc_array = malloc(proc_array_size * sizeof(int));

    if (rank == MASTER) {
        int *arr = malloc(size * sizeof(int));
        int *sums = malloc(proc * sizeof(int));
        for (int i = 0; i < size; i++) {
            arr[i] = i;
        }

        sums[0] = calculate_sum(arr, proc_array_size);

        for (int i = 1; i < proc; i++) {
            MPI_Send(&arr[i * proc_array_size], proc_array_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < proc; i++) {
            MPI_Recv(&sums[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
        }

        int sum = 0;
        for (int i = 0; i < proc; i++) {
            sum += sums[i];
        }

        printf("Sum = %d\n", sum);


    } else {
        MPI_Recv(proc_array, proc_array_size, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        int sum = calculate_sum(proc_array, proc_array_size);
        MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}