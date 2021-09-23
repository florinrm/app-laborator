#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_THREADS 4

// scriere in fisier
void write_file(char *filename, char *type, double time, long sum) {
    FILE *file = fopen(filename, "w+");
    fprintf(file, "Type: %s\nTime: %lf\nSum: %ld", type, time, sum);
    fclose(file);
}

int main() {
    FILE *file = fopen("input.txt", "r");
    int size, i;
    int *numbers;

    long sum1 = 0, sum2 = 0, sum3 = 0;
    double t1, t2;
    double time1, time2, time3;
    omp_set_num_threads(NO_THREADS);

    // de paralelizat toata bucata de mai jos

    // fiti atenti la partea de citire din fisier
    fscanf(file, "%d", &size);
    numbers = malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        fscanf(file, "%d", &numbers[i]);
    }
    fclose(file);

    // de paralelizat in 3 moduri - atomic, critical si reduction, de masurat timpii de executie
    for (i = 0; i < size; i++) {
        sum1 += numbers[i];
    }

    // TODO: de scris timpii de executie in 3 fisiere folosind sections (fiecare scriere intr-un section)

    return 0;
}