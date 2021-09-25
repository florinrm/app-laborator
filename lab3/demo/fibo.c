#include <stdio.h>
#include <omp.h>

int fib(int n) {
    int i, j;

    printf("n = %d | Thread id = %d\n", n, omp_get_thread_num());
 
    if (n < 2) {
        return n;
    }
 
    #pragma omp task shared(i)
    i = fib(n-1);
    
    #pragma omp task shared(j)
    j = fib(n-2);
    
    #pragma omp taskwait
    return i + j;
}


int main() {
    int n = 10;
    omp_set_num_threads(4);
    
    #pragma omp parallel shared(n)
    {
        #pragma omp single
        printf ("fib(%d) = %d\n", n, fib(n));
    }
}