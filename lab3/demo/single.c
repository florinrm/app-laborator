#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void f1() {
    for (int i = 0; i < 10; i++) {
        printf("f1 - iteration %d\n", i);
        sleep(1);
    }
}

void f2() {
    for (int i = 0; i < 10; i++) {
        printf("\tf2 - iteration %d\n", i);
        sleep(1);
    }
}

int main() {
    int i;

    #pragma omp parallel for ordered private(i)
    for (i = 0; i < 10; i++) {
        printf("** iteration %d thread no. %d\n", i, omp_get_thread_num());

        #pragma omp ordered
        printf("iteration %d thread no. %d\n", i, omp_get_thread_num());
    }

    int sum = 0;
    #pragma omp parallel
    {
        #pragma omp atomic
        sum += 2;
    }
    printf("sum = %d\n", sum);

    #pragma omp parallel for lastprivate(i)
    for (i = 0; i < 20; i++) {
        // do stuff
    }
    printf("Value of i = %d - after lastprivate | before firstprivate\n", i);

    #pragma omp parallel firstprivate(i)
    {
        #pragma omp single
        printf("Value of i = %d before for - firstprivate\n", i);

        #pragma omp for
        for (i = 0; i < 10; i++) {
            printf("i = %d\n", i);
        }

        #pragma omp single
        printf("Value of i = %d | after for - firstprivate\n", i);
    }
    printf("Value of i = %d | after parallel block\n", i);

    return 0;
}