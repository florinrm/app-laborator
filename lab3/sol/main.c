#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_THREADS 4

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
    
    #pragma omp parallel shared(size, sum1, sum2, sum3, numbers)
    {
        #pragma omp master
        {
            fscanf(file, "%d", &size);
            numbers = malloc(size * sizeof(int));

            for (int i = 0; i < size; i++) {
                fscanf(file, "%d", &numbers[i]);
            }
            fclose(file);
        }

        #pragma omp barrier

        #pragma omp master
        {
            t1 = omp_get_wtime();
        }

        #pragma omp for private(i)
        for (i = 0; i < size; i++) {
            #pragma omp critical
            sum1 += numbers[i];
        }

        #pragma omp master
        {
            t2 = omp_get_wtime();
            time1 = t2 - t1;
            printf("Total critical sum = %ld | Total execution time = %lf\n", sum1, time1);
            t1 = omp_get_wtime();
        }

        #pragma omp for private(i)
        for (i = 0; i < size; i++) {
            #pragma omp atomic
            sum2 += numbers[i];
        }

        #pragma omp master
        {
            t2 = omp_get_wtime();
            time2 = t2 - t1;
            printf("Total atomic sum = %ld | Total execution time = %lf\n", sum2, time2);
            t1 = omp_get_wtime();
        }

        #pragma omp for private(i) reduction(+:sum3)
        for (i = 0; i < size; i++) {
            sum3 += numbers[i];
        }

        #pragma omp master
        {
            t2 = omp_get_wtime();
            time3 = t2 - t1;
            printf("Total reduction sum = %ld | Total execution time = %lf\n", sum3, time3);
        }

        #pragma omp barrier

        #pragma omp sections
        {
            #pragma omp section
            {
                write_file("out1.txt", "critical", time1, sum1);
            }

            #pragma omp section
            {
                write_file("out2.txt", "atomic", time2, sum2);
            }

            #pragma omp section
            {
                write_file("out3.txt", "reduction", time3, sum3);
            }
        }
    }

    return 0;
}