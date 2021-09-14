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
    // daca avem mai multe sectiuni - un thread ruleaza o sectiune
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            f1();
            printf("Thread no. %d\n", omp_get_thread_num());
        }

        #pragma omp section
        {
            f2();
            printf("Thread no. %d\n", omp_get_thread_num());
        }
    }

    printf("\n");

    // aceeasi chestie ca mai sus, altfel formulata
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                f1();
                printf("Thread no. %d\n", omp_get_thread_num());
            }

            #pragma omp section
            {
                f2();
                printf("Thread no. %d\n", omp_get_thread_num());
            }
        }
    }

    printf("\n");

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                f1();
                printf("Thread no. %d\n", omp_get_thread_num());
            }

            #pragma omp section
            {
                f2();
                printf("Thread no. %d\n", omp_get_thread_num());
            }
        }

        #pragma omp sections
        {
            #pragma omp section
            {
                f1();
                printf("Thread no. %d\n", omp_get_thread_num());
            }

            #pragma omp section
            {
                f2();
                printf("Thread no. %d\n", omp_get_thread_num());
            }
        }
    }

    return 0;
}