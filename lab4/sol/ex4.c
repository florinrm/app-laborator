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
    int proc_arr_size = size / proc;

    int *arr = NULL;
    int *proc_arr = malloc(proc_arr_size * sizeof(int));
    int *sum_proc = NULL;

    if (rank == MASTER) {
        arr = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            arr[i] = i;
        } 
    }

    MPI_Scatter(arr, proc_arr_size, MPI_INT, proc_arr,
              proc_arr_size, MPI_INT, 0, MPI_COMM_WORLD);

    int sum_proc_arr = calculate_sum(proc_arr, proc_arr_size);

    if (rank == MASTER) {
        sum_proc = malloc(proc * sizeof(int));
    }

    MPI_Gather(&sum_proc_arr, 1, MPI_INT, sum_proc,
              1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == MASTER) {
        int total_sum = 0;
        for (int i = 0; i < proc; i++) {
            total_sum += sum_proc[i];
        }

        printf("Total sum = %d\n", total_sum);
    }

    MPI_Finalize();
    return 0;
}