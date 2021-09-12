#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

int w(int n) {
    int s = 0;
    for (int i = 0; i < n; i++) {
        s++;
    }
}

int main(int argc, char** argv) {
    int i;
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for private(i) schedule(static, 4)
    for (i = 0; i < 16; i++) {
        w(i);
        printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
    }
    
    return 0;
}