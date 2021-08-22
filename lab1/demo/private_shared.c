#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    int a = 6, b = 9, c = 10;

    #pragma omp parallel private(a,b) shared(c) 
    {
        // privates set the scope of variables
        a = 1, b = 2, c = a + b; // cu private(a, b), aceste valori (la a si b) vor fi vizibile doar in acest bloc
        int tid = omp_get_thread_num();
        printf("In parallel block, in thread no %d: %d %d %d\n", tid, a, b, c); // printing 1 2 3
    }
    printf("%d %d %d\n", a, b, c); // printing 6 9 3

    #pragma omp parallel shared(c) 
    {
        a = 1, b = 2, c = a + b;
        int tid = omp_get_thread_num();
        printf("In parallel block, in thread no %d: %d %d %d\n", tid, a, b, c); // printing 1 2 3
    }
    printf("%d %d %d\n", a, b, c); // printing 1 2 3

    c = 10;

    #pragma omp parallel 
    {
        a = 1, b = 2, c = a + b;
        int tid = omp_get_thread_num();
        printf("In parallel block, in thread no %d: %d %d %d\n", tid, a, b, c); // printing 1 2 3
    }
    printf("%d %d %d\n", a, b, c); // printing 1 2 3

    return 0;
}