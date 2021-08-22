#include <stdio.h>
#include <omp.h>

int main (int argc, char** argv) {
    int thread_id, sum = 0;
    #pragma omp parallel private(thread_id) shared(sum)
    {
        thread_id = omp_get_thread_num();
        #pragma omp critical
        sum += thread_id;
    }
    printf("%d\n",sum);
    
    return 0;
}