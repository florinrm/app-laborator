#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

#include "queue.h"

/* global queue */
extern struct queue_struct thr_queue;

#define SIZE 20
int a[SIZE], b[SIZE], c[SIZE];

int queue_terminate_workers(struct queue_struct *queue) {
    if ((queue == NULL) || (queue->valid != QUEUE_VALID))
        return EINVAL;
  
    pthread_mutex_lock(&queue->qlock);
    queue->qexit = 1;

    /* wakeup any waiting workers */
    pthread_cond_broadcast(&queue->worker_cv);
    pthread_mutex_unlock(&queue->qlock);

    return 0;
}

void queue_wait_workers(struct queue_struct *queue, int nthreads)
{
    if ((queue == NULL) || (queue->valid != QUEUE_VALID))
        return;
  
    pthread_mutex_lock(&queue->qlock);
    
    while (queue->worker_waiting != nthreads) {
        queue->boss_waiting++;
        pthread_cond_wait(&queue->boss_cv, &queue->qlock);
        queue->boss_waiting--;
    }

    pthread_mutex_unlock(&queue->qlock);
}

int optimal_number_of_threads() {
    // TODO - puneti numarul de threads
    return 0;
}
int optimal_queue_length() {
    // TODO - lungimea cozii
    return 0;
}

void process_tasks() {
    #define MAX_THREADS 64
    int i, ret, nthreads, max_queue_len;
    pthread_t threads[MAX_THREADS];
    pthread_attr_t attr;
    void worker(), boss();

    /* initialization code */
    /* ... */

    /* determine the number of threads to create and max queue length */
    nthreads = optimal_number_of_threads();
    max_queue_len = optimal_queue_length();

    /* initialize the queue */
    if (queue_init(&thr_queue, max_queue_len) != 0 ) {
        fprintf(stderr, "queue_init() error\n");
        exit(-1);
    }

    /* initialize any special thread attributes (if needed) */
    pthread_attr_init(&attr);
    /* ... */

    /* create worker threads */
    for (i = 1; (i < nthreads) && (i < MAX_THREADS); i++) {
        ret = pthread_create(&threads[i], &attr, (void *(*)())worker, NULL);
    }
    pthread_attr_destroy(&attr);

    /* we are in the boss thread */
    boss();

    /* request the workers terminate when all work is processed */
    if (queue_terminate_workers(&thr_queue) != 0) {
        fprintf(stderr, "dequeue() error\n");
        exit(-1);
    }

    /* wait for the workers to terminate */
    for (i = 1; (i < nthreads) && (i < MAX_THREADS); i++) {
        ret = pthread_join(threads[i], NULL);
    }

    /* perform any final processing and return */
}

int get_work(struct q_work_struct* w, int pos) {
    // TODO - atribuiti pozitia, a[poz] si b[poz] la structura - vedeti in queue.h
    
    return pos < SIZE ? 1 : 0;
}

void boss() {
    struct q_work_struct *ptr;
    int ret, i = 0;
  
    /* obtain work, return when complete */
    for (;;) {
        /* allocate a work structure */
        ptr = (struct q_work_struct *) malloc (sizeof(struct q_work_struct));
        if (ptr == NULL)
            break;
    
        /* create/obtain work and fill in the work structure */
        ret = get_work(ptr, i);
        if (ret == 0) {
            free((void *)ptr);
            break;
        }

        /* queue up the work */
        if (enqueue(&thr_queue, ptr) != 0) {
            fprintf(stderr, "enqueue() error\n");
            exit(-1);
        }
        
        ++i;
    }
}

void worker() {
    struct q_work_struct *ptr;
  
    /* go into an infinite loop processing work received from
    * the work queue. If we are supposed to terminate, we
    * will never return from one of the calls to dequeue();
    */
    for (;;) {
        /* obtain the next work request */
        ptr = dequeue(&thr_queue);
        if (ptr == NULL) {
            fprintf(stderr, "dequeue() error\n");
            exit(-1);
        }
    
        /* process the work request */
        /* ... */

        // TODO - aici se va face adunarea dintre a[poz] si b[poz] (c[poz] = a[poz] + b[poz])

        /* release memory for work request */
        free((void *)ptr);
    }
}

int main() {
    for (int i = 0; i < SIZE; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    process_tasks();

    for (int i = 0; i < SIZE; i++) {
        printf("%d ", c[i]);
    }
    printf("\n");

    return 0;
}

  
