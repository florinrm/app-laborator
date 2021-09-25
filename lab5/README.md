# Laboratorul 5 - pthreads
## Despre pthreads
## Implementarea unui program paralel în pthreads
### Includere și compilare
### Crearea și terminarea thread-urilor
## Elemente de sincronizare
### Mutex
Un mutex (mutual exclusion) este folosit pentru a delimita și pentru a proteja o zonă critică, unde au loc, de regulă, operații de citire și de scriere. Un singur thread intră în zona critică (o rezervă pentru el - lock), unde se execută instrucțiuni, iar celelalte thread-uri așteaptă ca thread-ul curent să termine de executat instrucțiunile din zona critică. După ce thread-ul curent termină de executat instrucțiuni în zona critică, aceasta o eliberează (unlock) și următorul thread urmează aceiași pași.

Pentru zonele critice în pthreads folosim `pthread_mutex_t`, care reprezintă un mutex, care asigură faptul că un singur thread accesează zona critică la un moment dat, thread-ul deținând lock-ul pe zona critică în momentul respectiv, și că celelalte thread-uri care nu au intrat încă în zona critică așteaptă eliberarea lock-ului de către thread-ul aflat în zona critică în acel moment.

Funcții pentru mutex:
- crearea unui mutex: `int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);`
- lock pe mutex: `int pthread_mutex_lock(pthread_mutex_t *mutex);`
- unlock pe mutex: `int pthread_mutex_unlock(pthread_mutex_t *mutex);`
- distrugerea unui mutex: `int pthread_mutex_destroy(pthread_mutex_t *mutex);`


Exemplu de folosire:
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2

int a = 0;
pthread_mutex_t mutex;

void *f(void *arg)
{
    // facem lock pe mutex
	pthread_mutex_lock(&mutex);
    // zona critica
	a += 2;
    // facem unlock pe mutex
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	int i;
	void *status;
	pthread_t threads[NUM_THREADS];
	int arguments[NUM_THREADS];

    // cream mutexul
	pthread_mutex_init(&mutex, NULL);

	for (i = 0; i < NUM_THREADS; i++) {
		arguments[i] = i;
		pthread_create(&threads[i], NULL, f, &arguments[i]);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], &status);
	}

    // distrugem mutex-ul
	pthread_mutex_destroy(&mutex);

	printf("a = %d\n", a);

	return 0;
}
```
### Barieră
Bariera este folosită atunci când dorim să sincronizăm thread-urile încât să ajungă (să se sincronizeze) în același punct. Mai concret, ea asigură faptul că niciun thread, gestionat de barieră, nu trece mai departe de zona în care aceasta este amplasată decât atunci când toate thread-urile gestionate de barieră ajung în aceeași zonă. În pthreads folosim structura `pthread_barrier_t` pentru barieră.

Funcții:
- crearea unei bariere: `int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);`
- așteptarea thread-urilor la barieră: `int pthread_barrier_wait(pthread_barrier_t *barrier);`
- distrugerea unei bariere: `int pthread_barrier_destroy(pthread_barrier_t *barrier);`

Exemplu de folosire:
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 8

pthread_barrier_t barrier;

void *f(void *arg)
{
    int index = *(int *) arg;
	
    printf("Before barrier - thread %d\n", index);
    pthread_barrier_wait(&barrier);
    printf("After barrier - thread %d\n", index);

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	int i;
	void *status;
	pthread_t threads[NUM_THREADS];
	int arguments[NUM_THREADS];

	pthread_barrier_init(&barrier, NULL, NUM_THREADS);

	for (i = 0; i < NUM_THREADS; i++) {
		arguments[i] = i;
		pthread_create(&threads[i], NULL, f, &arguments[i]);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], &status);
	}
	pthread_barrier_destroy(&barrier);
	return 0;
}
```
### Semafor
### Variabile condiție
## Modele de programare
### Boss worker
### Work crew 
### Pipeline
## Exerciții