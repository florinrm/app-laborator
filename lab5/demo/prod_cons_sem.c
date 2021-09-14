/*
 * implementarea problemei producator consumator folosind semafor
 */

#define _REENTRANT    1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS    50
#define CONSUMER        0
#define PRODUCER        1

#define BUF_LEN         3

static sem_t mutex;
static sem_t full_sem;     /* semafor contor al elementelor pline */
static sem_t empty_sem;    /* semafor contor al elementelor goale */

static char buffer[BUF_LEN];
static int buf_cnt = 0;

static void *producer_func (void *arg);
static void *consumer_func (void *arg);
static void my_pthread_sleep (int millis);

int main (void)
{
        int i;
        int type;
        pthread_t tid_v[NUM_THREADS];
        pthread_attr_t attr;

        /* semaforul pe post de mutex este initial deschis */
        sem_init (&mutex, 0, 1);

        sem_init (&full_sem, 0, 0);
        sem_init (&empty_sem, 0, 3);

        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

        srand (time (NULL));
        for (i = 0; i < NUM_THREADS; i++) {
                type = rand () % 2;
                if (type == CONSUMER)
                        pthread_create (&tid_v[i], &attr, consumer_func, NULL);
                else
                        pthread_create (&tid_v[i], &attr, producer_func, NULL);
        }

        for (i = 0; i < NUM_THREADS; i++)
                pthread_join (tid_v[i], NULL);

        return 0;
}

static void my_pthread_sleep (int millis)
{
        struct timeval timeout;

        timeout.tv_sec = millis / 1000;
        timeout.tv_usec = (millis % 1000) * 1000;

        select (0, NULL, NULL, NULL, &timeout);
}

static void *producer_func (void *arg)
{
        sem_wait (&empty_sem);

        sem_wait (&mutex);

        buffer[buf_cnt] = 'a';
        buf_cnt++;
        printf ("Produs un element.\n");

        sem_post (&mutex);

        my_pthread_sleep (rand () % 1000);

        sem_post (&full_sem);

        return NULL;
}

static void *consumer_func (void *arg)
{
        char elem;

        sem_wait (&full_sem);

        sem_wait (&mutex);

        buf_cnt--;
        elem = buffer[buf_cnt];
        printf ("Consumat un element.\n");

        sem_post (&mutex);

        my_pthread_sleep (rand () % 1000);

        sem_post (&empty_sem);

        return NULL;
}