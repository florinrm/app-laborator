#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

const int values[4] = {4, 11, 13, 14};

int w(int n) {
    long s = 0;
    long size = 10000000;

    for (int i = 0; i < 4; i++) {
        if (n == values[i]) {
            size *= 3;
        }
    }

    for (int i = 0; i < 20; i++) {
        for (int k = 0; k < size; k++) {
            s++;
        }
    }
}

int main(int argc, char** argv) {
    int i;
    double t1, t2;
    omp_set_num_threads(NUM_THREADS);

    t1 = omp_get_wtime();
    #pragma omp parallel for private(i) schedule(guided, 2)
    for (i = 0; i < 16; i++) {
        w(i);
        printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
    }
    t2 = omp_get_wtime();

    printf("Total execution time = %lf\n", (t2 - t1));
    
    return 0;
}