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
Un semafor este un element de sincronizare, care reprezintă o generalizare a mutex-ului. Semaforul are un contor care este incrementat la intrarea unui thread în zona de cod critică și care e decrementat când thread-ul respectiv iese din zona critică (contorul nu poate fi negativ în pthreads).

Pentru a folosi semafoare în pthreads ne folosim de structura `sem_t`, pentru care trebuie să includem biblioteca `semaphore.h`.

Semafoarele POSIX sunt de două tipuri:
- cu nume - sincronizare între procese diferite
- fără nume - sincronizare între thread-urile din cadrul aceluiași proces

Funcții:
- `int sem_init(sem_t *sem, int pshared, unsigned int value);` - inițiere semafor
- `int sem_destroy(sem_t *sem);` - distrugere semafor
- `int sem_post(sem_t *sem);` - acquire
- `int sem_wait(sem_t *sem);` - release

Exemplu folosire - producer - consumer:
```c
#define _REENTRANT    1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS    50
#define CONSUMER        0
#define PRODUCER        1

#define BUF_LEN         3

pthread_mutex_t mutex;
sem_t full_sem;     // semafor contor al elementelor pline
sem_t empty_sem;    // semafor contor al elementelor goale

char buffer[BUF_LEN];
int buf_cnt = 0;

void my_pthread_sleep (int millis) {
    struct timeval timeout;

    timeout.tv_sec = millis / 1000;
    timeout.tv_usec = (millis % 1000) * 1000;

    select (0, NULL, NULL, NULL, &timeout);
}

void *producer_func (void *arg) {
    sem_wait (&empty_sem);

    pthread_mutex_lock (&mutex);

    buffer[buf_cnt] = 'a';
    buf_cnt++;
    printf ("Produs un element.\n");

	pthread_mutex_unlock (&mutex);

    my_pthread_sleep (rand () % 1000);

    sem_post (&full_sem);

    return NULL;
}

void *consumer_func (void *arg) {
    sem_wait (&full_sem);

    pthread_mutex_lock (&mutex);

    buf_cnt--;
    char elem = buffer[buf_cnt];
    printf ("Consumat un element: %c\n", elem);

    pthread_mutex_unlock (&mutex);

    my_pthread_sleep (rand () % 1000);

    sem_post (&empty_sem);

    return NULL;
}

int main () {
    int i;
    int type;
    pthread_t tid_v[NUM_THREADS];

    pthread_mutex_init (&mutex, NULL);

    sem_init (&full_sem, 0, 0);
    sem_init (&empty_sem, 0, 3);

    srand (time (NULL));
    for (i = 0; i < NUM_THREADS; i++) {
        type = rand () % 2;
        if (type == CONSUMER) {
        	pthread_create (&tid_v[i], NULL, consumer_func, NULL);
		} else {
            pthread_create (&tid_v[i], NULL, producer_func, NULL);
        }
	}

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join (tid_v[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

    return 0;
}
```
### Variabile condiție
Variabilele condiție reprezintă o structură de sincronizare, care au asociat un mutex, și ele au un sistem de notificare a thread-urilor, astfel încât un thread să fie blocat până când apare o notificare de la alt thread. Pentru a putea folosi variabile condiție în pthreads ne folosim de structura `pthread_cond_t`.

Variabilele condiție sunt folosite pentru a bloca thread-ul curent (mutexul și semaforul blochează celelalte thread-uri). Acestea permit unui thread să se blocheze până când o condiție devine adevărată, moment când condiția este semnalată de thread că a devenit adevărată și thread-ul / thread-urile blocate de condiție își reiau activitatea o variabilă condiție va avea mereu un mutex pentru a avea race condition, care apare când un thread 0 se pregătește să aștepte la variabila condiție și un thread 1 semnalează condiția înainte ca thread-ul 0 să se blocheze

Funcții:
- `int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);` - inițializare variabilă condiție
- `int pthread_cond_destroy(pthread_cond_t *cond);` - distrugere variabilă condiție
- `pthread_cond_t cond = PTHREAD_COND_INITIALIZER;` - inițializare statică a unei variabile condiție (atribute default, nu e nevoie de distrugere / eliberare)
- `int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);` - blocarea unui thread care așteaptă după o variabilă condiție
- `int pthread_cond_signal(pthread_cond_t *cond);` - deblocarea unui thread
- `int pthread_cond_broadcast(pthread_cond_t *cond);` - deblocarea tuturor thread-urilor blocate

Exemplu folosire - producer - consumer:
```c
#define _REENTRANT    1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS    50
#define CONSUMER        0
#define PRODUCER        1

#define BUF_LEN         3

pthread_mutex_t mutex; // folosit pentru incrementarea si decrementarea marimii buffer-ului
pthread_cond_t full_cond; // cand buffer-ul este gol
pthread_cond_t empty_cond; // cand buffer-ul este plin

char buffer[BUF_LEN];
int buf_cnt = 0;

void my_pthread_sleep(int millis) {
	struct timeval timeout;

	timeout.tv_sec = millis / 1000;
	timeout.tv_usec = (millis % 1000) * 1000;

	select (0, NULL, NULL, NULL, &timeout);
}

void *producer_func (void *arg) {
	pthread_mutex_lock (&mutex);
	
	// cat timp buffer-ul este plin, producatorul asteapta
	while (buf_cnt == BUF_LEN) {
		pthread_cond_wait (&full_cond, &mutex);
	}

	buffer[buf_cnt] = 'a';
	buf_cnt++;
	printf ("Produs un element.\n");

	pthread_cond_signal (&empty_cond);
	my_pthread_sleep (rand () % 1000);

	pthread_mutex_unlock (&mutex);

	return NULL;
}

void *consumer_func (void *arg) {
	pthread_mutex_lock (&mutex);

	// cat timp buffer-ul este gol, consumatorul asteapta
	while (buf_cnt == 0) {
		pthread_cond_wait (&empty_cond, &mutex);
	}

	buf_cnt--;
	char elem = buffer[buf_cnt];
	printf ("Consumat un element: %c\n", elem);

	pthread_cond_signal (&full_cond);
	my_pthread_sleep (rand () % 1000);

	pthread_mutex_unlock (&mutex);

	return NULL;
}

int main() {
	int i;
	int type;
	pthread_t tid_v[NUM_THREADS];

	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&full_cond, NULL);
	pthread_cond_init (&empty_cond, NULL);

	srand (time (NULL));
	for (i = 0; i < NUM_THREADS; i++) {
		type = rand () % 2;
		if (type == CONSUMER) {
			pthread_create (&tid_v[i], NULL, consumer_func, NULL);
		} else {
			pthread_create (&tid_v[i], NULL, producer_func, NULL);
		}
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join (tid_v[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	return 0;
}
```
## Modele de programare
### Boss worker
### Work crew 
### Pipeline
## Exerciții
1) **(2.5 puncte)** Implementați bariera folosind variabile condiție pe baza scheletului.

2) **(2.5 puncte)** Implementați bariera folosind semafoare pe baza scheletului.

3) **(2.5 puncte)** Rezolvați boss worker pe baza scheletului.

4) **(2.5 puncte)** Rezolvați work crew pe baza scheletului.

5) **(opțional)** Rezolvați problema bărbierului pe baza scheletului.

6) **(opțional)** Rezolvați problema Readers-Writers pe baza scheletului.

7) **(opțional)** Rezolvați problema filosofilor pe baza scheletului.