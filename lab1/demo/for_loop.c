#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    int i, x[20];
    #pragma omp parallel private(i) shared(x) 
    {
        #pragma omp for 
            for (i = 0; i < 20; i++) {
                x[i] = i;
                printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
            }
    }

    printf("\n");

    // o alta forma, (aproape) aceeasi branza - detalii legate de loops in laboratorul 2
    #pragma omp parallel for private(i) shared(x) 
        for (i = 0; i < 20; i++) {
            x[i] = i;
            printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
        }
    
    return 0;
}