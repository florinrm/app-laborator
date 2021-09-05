#include <stdio.h>
#include <omp.h>

int main (int argc, char** argv) {
    #pragma omp parallel 
    {
        printf("First print by %d\n", omp_get_thread_num());
        #pragma omp barrier
        printf("Second print by %d\n", omp_get_thread_num());
    }
    
    return 0;
}