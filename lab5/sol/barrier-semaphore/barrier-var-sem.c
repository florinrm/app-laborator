#include <stdio.h>
#include <pthread.h>
 
#define NUM_THREADS 5

// copyright echipa de Sisteme de operare UPB
 
// implementarea unei bariere *non-reutilizabile* cu variabile de condiție
struct my_barrier_t {
    // mutex folosit pentru a serializa accesele la datele interne ale barierei
    pthread_mutex_t lock;
 
    // variabila de condiție pe care se așteptă sosirea tuturor firelor de execuție
    pthread_cond_t  cond;
 
    // număr de fire de execuție care trebuie să mai vină pentru a elibera bariera
    int nr_still_to_come;
};
 
struct my_barrier_t bar;
 
void my_barrier_init(struct my_barrier_t *bar, int nr_still_to_come) {
    pthread_mutex_init(&bar->lock, NULL);
    pthread_cond_init(&bar->cond, NULL);
 
    // câte fire de execuție sunt așteptate la barieră
    bar->nr_still_to_come = nr_still_to_come;
}
 
void my_barrier_destroy(struct my_barrier_t *bar) {
    pthread_cond_destroy(&bar->cond);
    pthread_mutex_destroy(&bar->lock);  
}
 
void *thread_routine(void *arg) {
    int thd_id = (int) arg;
 
    // înainte de a lucra cu datele interne ale barierei trebuie să preluam mutex-ul
    pthread_mutex_lock(&bar.lock);
 
    printf("thd %d: before the barrier\n", thd_id);
 
    // suntem ultimul fir de execuție care a sosit la barieră?
    int is_last_to_arrive = (bar.nr_still_to_come == 1);
    // decrementăm numarul de fire de execuție așteptate la barieră
    bar.nr_still_to_come --;
 
    // cât timp mai sunt fire de execuție care nu au ajuns la barieră, așteptăm.
    while (bar.nr_still_to_come != 0)
        // mutex-ul se eliberează automat înainte de a incepe așteptarea
        pthread_cond_wait(&bar.cond, &bar.lock); 
 
    // ultimul fir de execuție ajuns la barieră va semnaliza celelalte fire 
    if (is_last_to_arrive) {
        printf("    let the flood in\n");
        pthread_cond_broadcast(&bar.cond);
    }
 
    printf("thd %d: after the barrier\n", thd_id);
 
    // la ieșirea din funcția de așteptare se preia automat mutex-ul, care trebuie eliberat
    pthread_mutex_unlock(&bar.lock);
 
    return NULL;
}
 
int main(void) {
    int i;
    pthread_t tids[NUM_THREADS];
 
    my_barrier_init(&bar, NUM_THREADS);
 
    for (i = 0; i < NUM_THREADS; i++)
        pthread_create(&tids[i], NULL, thread_routine, (void *) i);
 
    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(tids[i], NULL);
 
    my_barrier_destroy(&bar);
 
    return 0;
}