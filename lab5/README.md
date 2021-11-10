# Laboratorul 5 - pthreads
## Despre pthreads
pthreads reprezintă o bibliotecă din C/C++, nativă Linux, prin care se pot implementa programe multithreaded.

Spre deosebire de OpenMP, pthreads este low-level și oferă o mai mare flexibilitate în ceea ce privește sincronizarea thread-urilor și distribuirea task-urilor către thread-uri.
## Implementarea unui program paralel în pthreads
### Includere și compilare
Pentru a putea folosi pthreads, este necesar să includem în program biblioteca `pthread.h`. De asemenea la compilare este necesar să includem flag-ul `-lpthread`:
```bash
gcc -o program program.c -lpthread
./program
```
### Crearea și terminarea thread-urilor
În pthreads, avem un thread principal, pe care rulează funcția main. Din thread-ul principal se pot crea thread-uri noi, care vor executa task-uri în paralel.

Pentru a crea thread-uri în pthreads, folosim funcția `pthread_create`:
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*thread_function) (void *), void *arg);
```
unde:
- thread - thread-ul pe care vrem să-l pornim
- attr - atributele unui thread (NULL - atribute default)
- thread_function - funcția pe care să o execute thread-ul
- arg - parametrul trimis la funcția executată de thread (dacă vrem să trimitem mai mulți parametri, îi împachetăm într-un struct

Exemplu de funcție pe care o execută un thread:
```c
void *f(void *arg) {
	// do stuff
	// aici putem să întoarcem un rezultat, dacă este cazul
    pthread_exit(NULL); // termină un thread - mereu apelat la finalul unei funcții executate de thread, dacă nu întoarcem un rezultat în funcție
}

```

Pentru terminarea thread-urilor, care vor fi "lipite înapoi" în thread-ul principal, folosim funcția `pthread_join`, care așteaptă terminarea thread-urilor:
```c
int pthread_join(pthread_t thread, void **retval);
```
unde:
- thread - thread-ul pe care îl așteptăm să termine
- retval - valoarea de retur a funcției executate de thread (poate fi NULL)

Exemplu de program scris folosind pthreads:
```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 2
 
void *f(void *arg)
{
    long id = *(long*) arg;
    printf("Hello World din thread-ul %ld!\n", id);
    return NULL;
}
 
int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int r;
    long id;
    void *status;
    long arguments[NUM_THREADS];
 
    for (id = 0; id < NUM_THREADS; id++) {
        arguments[id] = id;
        r = pthread_create(&threads[id], NULL, f, (void *) &arguments[id]);
 
        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", id);
            exit(-1);
        }
    }
 
    for (id = 0; id < NUM_THREADS; id++) {
        r = pthread_join(threads[id], &status);
 
        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", id);
            exit(-1);
        }
    }
 
    return 0;
}
```

În caz că dorim să trimitem mai mulți parametri funcției executate de threads, facem un struct, în care incapsulăm datele, și îl trimitem ca parametru al funcției executate de threads.

Exemplu:
```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 8

struct pair {
    int first, second;
};
 
void *f(void *arg)
{
    struct pair info = *(struct pair*) arg;
    printf("First = %d; second = %d\n", info.first, info.second);
    pthread_exit(NULL);
}
 
int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int r;
    long id;
    void *status;
    struct pair arguments[NUM_THREADS];
 
    for (id = 0; id < NUM_THREADS; id++) {
        arguments[id].first = id;
        arguments[id].second = id * 2;
        r = pthread_create(&threads[id], NULL, f, (void *) &arguments[id]);
 
        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", id);
            exit(-1);
        }
    }
 
    for (id = 0; id < NUM_THREADS; id++) {
        r = pthread_join(threads[id], &status);
 
        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", id);
            exit(-1);
        }
    }
 
    pthread_exit(NULL);
}
```

Când dorim să paralelizăm operații efectuate pe arrays, fiecărui thread îi va reveni o bucată din array, pe acea bucată executând funcția atribuită lui (thread-ului).

Formula de împărțire:
- `start_index = id * (double) n / p`
- `end_index = min(n, (id + 1) * (double) n / p))`, unde id = id-ul thread-ului, n = dimensiunea array-ului, p = numărul de threads

![thread_array](../media/lab5/thread_array.png)

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
Modelul boss-worker este o versiune generalizată a modelului producer-consumer, unde avem un thread cu rolul de boss / master, care produce task-uri, și restul thread-urilor au rol de worker, ele având rolul de a executa task-urile produse de thread-ul boss.

Task-urile sunt puse într-o coadă de către thread-ul boss și preluate de către thread-urile worker, care le execută.

În general avem un singur thread boss, dar putem avea mai multe thread-uri de tip boss.

Probleme ce pot apărea:
- performanța când sunt multe task-uri executate prin refolosirea thread-urilor
- resursele limitate pentru execuția thread-urilor
- probleme de sincronizare

Boss worker se poate implementa folosind:
- două variabile condiționale
	- prima folosită de thread-urile worker să aștepte când coada este goală. Dacă nu e goală, workers semnalează thread-urilor boss și iau din coadă
	- a doua folosită de thread-urile boss să aștepte când coada este plină. Dacă nu e plină, boss semnalează thread-urilor worker și pune în coadă.
- un mutex - protejează coada, variabilele condiționale și contoarele
- o coadă - sincronizare când se pun / preiau date de către thread-uri
	- dimensiunea cozii poate să fie:
		- statică - sunt necesare două variabile condiție (una pentru boss, alta pentru workers)
		- dinamică - o necesară o variabilă condiție pentru workers (când coada e goală), dar are un dezavantaj pentru boss, mai precis acesta poate să fie supraîncărcat cu task-uri

- trei contoare:
	- numărul de task-uri în coadă
	- numărul de thread-uri worker în așteptare
	- numărul de thread-uri boss în așteptare

Thread-urile acționează într-o buclă continuă în ceea ce privește crearea și execuția task-urilor și ele nu își termină execuția când coada este goală, astfel trebuie să avem o modalitate de terminare a execuției thread-urilor boss și workers.

Putem face terminarea thread-urilor în două moduri:
- un task special de tip ”exit” pentru thread-urile worker, care se vor opri când primesc acest task (ordinea task-urilor să fie FIFO)
- coada să aibă un flag de exit (ea trebuie să fie goală), setat de către thread-ul boss. Thread-urile worker se opresc când văd că coada este goală și flag-ul de exit setat pe true, iar apoi thread-ul boss se oprește

### Work crew 
Work crew reprezintă un model de programare paralelă, unde avem un thread principal și N - 1 thread-uri, care sunt controlate de către thread-ul principal, care le creează. Cele N - 1 thread-uri sunt thread-uri de tip worker, care execută task-uri distribuite de către thread-ul principal, pe care, de asemenea, le execută, după ce thread-urile worker au terminat execuția lor. 
## Probleme de sincronizare - opțional
### Producer - consumer
Problema se referă la două thread-uri: producător și consumator. Producătorul inserează date într-un buffer, iar consumatorul extrage date din acel buffer. Buffer-ul are o dimensiune prestabilită, astfel că:
- producătorul nu poate insera date dacă buffer-ul este plin
- consumatorul nu poate extrage date dacă buffer-ul este gol
- producătorul și consumatorul nu pot acționa simultan asupra buffer-ului

O implementare corectă a problemei presupune asigurarea faptului că nu vor exista situații de deadlock, adică situații în care cele două thread-uri așteaptă unul după celălalt, neexitând posibilitatea de a se debloca.

Această problemă se poate rezolva în mai multe moduri (rezolvările sunt mai sus, în cadrul textului laboratorului):
- folosind semafoare
- folosind variabile condiție

Pseudocod varianta cu semafoare:
```
T[] buffer = new T[k];
semaphore gol(k);
semaphore plin(0);
mutex mutex;

producer(int id) {
    T v;
    while (true) {
        v = produce();
		gol.acquire();

		mutex.lock();
		buf.add(v);
		mutex.unlock();
			
		plin.release();
    }
}

consumer(int id) {
    T v;
    while (true) {
        plin.acquire();

		mutex.lock();
		v = buf.poll();
		mutex.unlock();

		gol.release();
		consume(v);
    }
}
```
### Problema cititorilor și a scriitorilor (Readers - Writers)
Avem o zonă de memorie asupra căreia au loc mai multe acțiuni de citire și de scriere. Această zonă de memorie este partajată de mai multe thread-uri, care sunt de două tipuri: cititori (care execută acțiuni de citire din zona de memorie) și scriitori (care execută acțiuni de scriere în zona de memorie).

În această privință avem niște constrângeri:
- un scriitor poate scrie în zona de memorie doar dacă nu avem cititori care citesc din zona respectivă în același timp și dacă nu avem alt scriitor care scrie în același timp în aceeași zonă de memorie.
- un cititor poate să citească în zona de memorie doar dacă nu există un scriitor care scrie în zona de memorie în același timp, însă putem să avem mai mulți cititori care citesc în paralel în același timp din aceeași zonă de memorie.

Pentru această problemă avem două soluții:
- folosind excludere mutuală, cu prioritate pe cititori
- folosind sincronizare condiționată, cu prioritate pe scriitori

#### Soluția cu excludere mutuală
Folosind această soluție, un cititor nu va aștepta ca ceilalți cititori să termine de citit zona de memorie, chiar dacă avem un scriitor care așteaptă. Un scriitor poate să aștepte foarte mult, în caz că sunt foarte mulți scriitor, fapt ce poate duce la un fenomen numit writer's starvation.

De asemenea, nu poate să intre un scriitor cât timp există deja un scriitor care scrie în zona de memorie partajată.

Pseudocod:
```
// numărul de cititori care citesc simultan din resursa comună
int readers = 0; 

// mutex (sau semafor) folosit pentru a modifica numărul de cititori
mutex mutexNumberOfReaders; // sau semaphore mutexNumberOfReaders(1);

// semafor (sau mutex) folosit pentru protejarea resursei comune
semaphore readWrite(1); // sau mutex readWrite

reader (int id) {
    while (true)
        mutexNumberOfReaders.lock();
		readers = readers + 1;
			// dacă e primul cititor, atunci rezervăm zona de memorie încât să nu intre niciun scriitor
		if (readers == 1) {
			readWrite.acquire(); 
		};
		mutexNumberOfReaders.unlock();
		
			// citește din resursa comună;
			
		mutexNumberOfReaders.lock();
		readers = readers - 1;
			// dacă e ultimul cititor, eliberăm zona de de memorie din care s-a citit
		if (readers == 0) {
			readWrite.release();
		}
		mutexNumberOfReaders.unlock();
    }
}

writer (int id) {
    while (true) {
        // intră scriitorul în resursa comună
        readWrite.acquire();
        
        // scrie în resursa comună;
	
        // scriitorul eliberează resursa
        readWrite.release();
    }
}
```
#### Soluția cu sincronizare condiționată
Folosind această soluție, niciun cititor nu va intra în zona de memorie partajată cât timp există un scriitor care scrie în zona de memorie. De asemenea, nu poate să intre alt scriitor cât timp există un scriitor care se află în zona de memorie partajată.

```
// cititori care citesc din zona de memorie
int readers = 0;
// scriitori care scriu în zona de memorie
// (va fi doar unul, nu pot fi mai mulți scriitori care scriu simultan)
int writers = 0;

int waiting_readers = 0; // cititori care așteaptă să intre în zona de memorie
int waiting_writers = 0; // scriitori care așteaptă să intre în zona de memorie

// semafor folosit pentru a pune scriitori în așteptare, dacă avem un scriitor
// sau unul sau mai mulți cititori în zona de memorie (zona critică)
semaphore sem_writer(0);

// semafor folosit pentru a pune cititori în așteptare dacă avem un scriitor care scrie în zona de memorie
// sau dacă avem scriitori în așteptare (deoarece ei au prioritate față de cititori)
semaphore sem_reader(0);

// semafor folosit pe post de mutex pentru protejarea zonei de memorie (zona critică)
semaphore enter(1); 

reader (int id) {
    while(true) {
        enter.acquire();

        // dacă avem cel puțin un scriitor care scrie în resursa comună
        // sau dacă avem un scriitor în așteptare, cititorul așteaptă
		if (writers > 0 || waiting_writers > 0) {
			waiting_readers++;
			enter.release();
			sem_reader.acquire();
		}

			readers++;
		if (waiting_readers > 0) {
				// a venit încă un cititor în resursa comună,
				// ieșind din starea de așteptare
			
				waiting_readers--;
			sem_reader.release();
		} else if (waiting_readers == 0) {
			enter.release();
		}

		// citește din zona partajată
		enter.acquire();
			readers--;
			
			if (readers == 0 && waiting_writers > 0) {
			waiting_writers--;
			sem_writer.release();
		} else if (readers > 0 || waiting_writers == 0) {
			enter.release();
		}
    }
}

writer (int id) {
    while(true) {
        enter.acquire();
		
		if (readers > 0 || writers > 0) {
			waiting_writers++;
			enter.release();
			sem_writer.acquire();
		}

		writers++;

		enter.release();
	
        // scrie în zona partajată
	
        enter.acquire();

		writers--;
		if (waiting_readers > 0 && waiting_writers == 0) {
			waiting_readers--;
			sem_reader.release();
		} else if (waiting_writers > 0) {
			waiting_writers--;
			sem_writer.release();
		} else if (waiting_readers == 0 && waiting_writers == 0) {
			enter.release();
		}
    }
}
```
### Problema filosofilor
Problema se referă la mai mulți filozofi (thread-uri) așezați la o masă circulară. Pe masă se află N farfurii și N tacâmuri, astfel încât fiecare filozof are un tacâm în stânga și unul în dreapta lui. În timp ce stau la masă, filozofii pot face două acțiuni: mănâncă sau se gândesc. Pentru a mânca, un filozof are nevoie de două tacâmuri (pe care le poate folosi doar dacă nu sunt luate de către vecinii săi).

Rezolvarea trebuie să aibă în vedere dezvoltarea unui algoritm prin care să nu se ajungă la un deadlock (situația în care fiecare filozof ține câte un tacâm în mână și așteaptă ca vecinul să elibereze celălalt tacâm de care are nevoie).

Ca soluție, avem în felul următor: vom avea N lock-uri (având în vedere că avem N thread-uri), fiecare filosof va folosi câte două lock-uri. Pentru a evita deadlock-ul, totul va funcționa în felul următor:
- fiecare din primele N - 1 thread-uri va face lock mai întâi pe lock pe lock[i], apoi pe lock[i + 1], apoi execută o acțiune, apoi face release pe lock[i], apoi pe lock[i + 1].
- al N-lea thread va face lock mai întâi pe lock[0], apoi pe lock[N - 1] (deci invers față de restul thread-urilor), execută o acțiune, apoi face release pe lock[0], apoi pe lock[N - 1].

Pseudocod:
```
Lock[] locks = new Lock[N];

philosopher(int id) {
	while (true) {
		if (id != N - 1) {
			locks[id].lock();
			locks[id + 1].lock();
			// eat
			locks[id].release();
			locks[id + 1].release();
			// think
		} else {
			locks[0].lock();
			locks[N - 1].lock();
			// eat
			locks[0].release();
			locks[N - 1].release();
			// think
		}
	} 
}
```
### Problema bărbierului
Avem următoarea situație: avem o frizerie cu un bărbier (un thread), un scaun de bărbier, N scaune de așteptare și M clienți (M thread-uri).

La această problemă avem următoarele constrângeri:
- bărbierul doarme atunci când nu sunt clienți
- când vine un client, acesta fie trezește bărbierul, fie așteaptă dacă bărbierul este ocupat
- dacă toate scaunele sunt ocupate, clientul pleacă

```
int freeChairs = N;
semaphore clients(0);
semaphore barber_ready(0);
semaphore chairs(1); // sau mutex

barber() {
    while(true) {
        clients.acquire(); // se caută client; dacă există, el este chemat
		
		chairs.acquire(); // are client, un scaun este eliberat, modificăm freeChairs
			
		freeChairs++; // scaun eliberat

		barber_ready.release(); // bărbierul e gata să tundă
		chairs.release(); // freeChairs modificat

		// tunde bărbierul
    }
}

client(int id) {
    while(true) {
        chairs.acquire(); // vine un client și caută un scaun liber
        if (freeChairs > 0) {
			freeChairs--; // clientul a găsit scaun
				
			clients.release(); // bărbierul știe că s-a ocupat un scaun de un client
				
			chairs.release(); // freeChairs modificat
				
			barber_ready.acquire(); // clientul își așteaptă rândul la tuns
		} else {
			// nu sunt scaune libere
			chairs.release();
			// clientul pleacă netuns
		}
    }
}
```
## Exerciții
1) **(2.5 puncte)** Implementați bariera folosind variabile condiție pe baza scheletului.

2) **(2.5 puncte)** Implementați bariera folosind semafoare pe baza scheletului.

3) **(2.5 puncte)** Rezolvați boss worker pe baza scheletului.

4) **(2.5 puncte)** Folosind work crew, implementați însumarea elementelor dintr-o matrice, cu împărțirea matricei pe linii / coloane, fiecare thread cu liniile / coloanele sale, pe baza scheletului.

5) **(opțional)** Rezolvați problema bărbierului pe baza scheletului.

6) **(opțional)** Rezolvați problema Readers-Writers pe baza scheletului.

7) **(opțional)** Rezolvați problema filosofilor pe baza scheletului.
