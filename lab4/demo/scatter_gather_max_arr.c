#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0

const int chunks_per_process = 5; // numarul de elemente per proces

int* create_array (int size) {
    int* arr = (int*) malloc (size * sizeof(int));
    
    for (int i = 0; i < size; ++i) {
        arr[i] = i + 1;
    }

    return arr;
}

int* increaseEveryElement (int* arr, int size) {
    for (int i = 0; i < size; ++i) {
        arr[i]++;
    }

    return arr;
}

// aflam maximum dintr-un array
int maximum_array (int* arr, int size) {
    int elem = arr[0];

    for (int i = 0; i < size; ++i) {
        if (elem <= arr[i])
            elem = arr[i];
    }

    return elem;
}

int main (int argc, char **argv) {
    int rank, proc, a;

    // arr e array-ul trimis de un proces tuturor proceselor (o bucata din el la un proces)
    // process_arr este bucata de array primita de un proces
    // max_arr e un array de maximuri din subsecvente din array
    int* arr;
    int* process_arr;
    int* max_arr;
    
    // initializam procesele
    MPI_Init(&argc, &argv);
    
    // fiecare proces are un rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank
    // stabilim numarul de procese
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    // procesul root creeaza array-ul pe care il trimite (impartit) tuturor proceselor
    if (rank == ROOT) {
        arr = create_array(chunks_per_process * proc);
    }

    // alocam pentru bucata primita de fiecare proces de la root
    process_arr = (int*) malloc (chunks_per_process * sizeof(int));

    // acum imprastiem array-ul tuturor proceselor, inclusiv lui root, care primeste bucata sa din array
    MPI_Scatter(arr, chunks_per_process, MPI_INT, process_arr, chunks_per_process, MPI_INT, ROOT, MPI_COMM_WORLD);

    // calculam maximul pe bucata noastra de array (asta face fiecare proces in parte)
    int maximum = maximum_array(process_arr, chunks_per_process);

    if (rank == ROOT) {
        max_arr = (int*) malloc (proc * sizeof(int));
    }

    /*
    * acum fiecare proces isi trimite maximul sau aflat pe bucata sa primita din array
    * iar procesul root primeste acel maxim si il pune in array-ul pe maximuri locale
    * pe care aplica iar functia de maxim pentru a afla maximul dintre toate maximurile locale
    * fiecarui proces
    */
    MPI_Gather(&maximum, 1, MPI_INT, max_arr, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        // maximul aflat din array-ul de maximuri locale
        int max_maximum = maximum_array(max_arr, proc);
        printf("Maximum found with scatter and gather: %d\n", max_maximum);

        // maxim aflat pe tot array-ul original
        int total_max = maximum_array(arr, proc * chunks_per_process);
        printf("Maximum found in regular way - not distributed: %d\n", total_max);
    }

    if (rank == ROOT) {
        free(arr);
        free(max_arr);
    }

    free(process_arr);

    MPI_Finalize();
    return 0;
}