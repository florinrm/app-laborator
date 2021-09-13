#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

int c(int n) {
    long s = 0;
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < 10000000; k++) {
            s++;
        }
    }
}

int d() {
    long s = 0;
    for (int k = 0; k < 20; k++) {
        for (long i = 0; i < 10000000; i++) {
            s++;
        } 
    }
}

int main(int argc, char** argv) {
    omp_set_num_threads(NUM_THREADS);

    int i;
    #pragma omp parallel
    {
        #pragma omp for nowait private(i)
        for (i = 0; i < 16; i++) {
            c(i);
        }
        d();
    }
    
    return 0;
}