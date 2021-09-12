#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

int w(int n) {
    long s = 0;
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < 10000000; k++) {
            s++;
        }
    }
}

int main(int argc, char** argv) {
    int i;
    double t1, t2;
    omp_set_num_threads(NUM_THREADS);

    t1 = omp_get_wtime();
    #pragma omp parallel for private(i) schedule(static, 1)
    for (i = 0; i < 16; i++) {
        w(i);
        printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
    }
    t2 = omp_get_wtime();

    printf("Total execution time = %lf\n", (t2 - t1));
    
    return 0;
}