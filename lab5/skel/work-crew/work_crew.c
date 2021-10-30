#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int size = 10000;
int no_threads = 8;
int **A;
int *results;

int min(int a, int b) {
    return a <= b ? a : b;
}

void *do_sum(void* arg) {
    int tid = *(int*) arg;
    printf("%d\n", tid);
    
    int start = tid * ((double) size / no_threads);
    int end = min(size, (tid + 1) * ((double) size / no_threads));
    
    for (int i = start; i < end; i++) {
        float column_sum = 0;
        for (int j = 0; j < size; j++) {
            column_sum += A[i][j];
        }
        results[i] = column_sum;
    }
    
    return NULL;
}

int main(int argc, char const *argv[]) {
    int total_sum = 0;
    A = malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        A[i] = calloc(size, sizeof(int));
    }
    results = calloc(size, sizeof(int));
    
    if (A == NULL) {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    if (results == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            A[i][j] = i * 6 + j * 9;
        }
    }
    
    // worker threads
    pthread_t threads[no_threads];
    int tids[no_threads];
    
    // TODO

    for (int i = 0; i < size; i++) {
        total_sum += results[i];
    }

    printf("Total sum = %d\n", total_sum);
    
    return 0;
}
