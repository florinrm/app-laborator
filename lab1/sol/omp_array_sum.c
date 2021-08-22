#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 1000000

int main() {

    int a[SIZE];
    long sum = 0;
    int i;
    double t1, t2;
    
    // inițializare array
    #pragma omp parallel shared(a) private(i)
    {
        #pragma omp for
        for (int i = 0; i < SIZE; i++) {
            a[i] = i;
        }
    }

    // varianta 1 - cu critical (mutex)
    t1 = omp_get_wtime();
    #pragma omp parallel shared(a, sum) private(i)
    {
        #pragma omp for
        for (i = 0; i < SIZE; i++) {
            #pragma omp critical
            sum += a[i];
        }
    }
    t2 = omp_get_wtime();
    printf("Total sum = %ld | Total execution time = %lf\n", sum, (t2 - t1));

    sum = 0;

    // varianta 2 - cu reduction (atomicitate)
    t1 = omp_get_wtime();
    #pragma omp parallel reduction(+:sum) private(i) shared(a)
    {
        #pragma omp for
        for (i = 0; i < SIZE; i++) {
            sum += a[i];
        }
    }
    t2 = omp_get_wtime();
    printf("Total sum = %ld | Total execution time = %lf\n", sum, (t2 - t1));

    return 0;
}