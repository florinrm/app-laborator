#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    omp_set_num_threads(4);
    omp_set_nested(1);
    #pragma omp parallel 
    {
        int tid = omp_get_thread_num();
        printf("Hello world from thread number %d\n", tid);

    
        #pragma omp parallel 
        {
            int tid = omp_get_thread_num();
            printf("Frumos in anul 4, zice thread-ul %d\n", tid);
        }
    }

    return 0;
}