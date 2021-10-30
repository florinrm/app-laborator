#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#define CHAIRS 3
#define CLIENTS 7
    
pthread_mutex_t served_client_mutex;

sem_t barber_ready; 
sem_t client_ready;
sem_t modifySeats;

int chairs;
int total_clients;

int available_chairs;
int left_and_unserved_clients = 0;
time_t waiting_time_sum;

void *barber_function(void *idp) {    
    int counter = 0;
    
    for(;;) {
        // TODO

        // Increment by 1 the available seats
        available_chairs++;

        // TODO

        // Lock mutex "served_client_mutex - protect service from the same barber from other threads (clients)
        pthread_mutex_lock(&served_client_mutex);

        // serve client
        service_client();    

        // Unlock mutex "served_client_mutex - finished service
        pthread_mutex_unlock(&served_client_mutex);
        
        printf("Client %d was served.\n", counter);
        counter++; 

        if (counter == (total_clients - left_and_unserved_clients)) {
            break;
        }

    }
    pthread_exit(NULL);    
}

// dummy function for serving a client
void service_client() {
    int s = rand() % 401;
    s = s * 1000;
    printf("brrr, cutting client's hair\n");
    usleep(s);
    printf("done cutting hair\n");
}

void *customer_function(void *idp) {  
    int client_id = *(int*) idp;
    // TODO

    // If there is available seat
    if (available_chairs >= 1) {
        // Occupy a seat
        available_chairs--;

        printf("Client %d is waiting.\n", client_id);
        printf("Available seats: %d\n", available_chairs);
        
        // TODO

        printf("Client %d is being served. \n", client_id);        
    } else {
        // Unlock semaphore "modifySeats"
        // TODO
        left_and_unserved_clients++;
        printf("Client %d left.\n", client_id);
    }
        
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));   

    // barber
    pthread_t barber;

    // clients
    pthread_t clients[CLIENTS];

    pthread_mutex_init(&served_client_mutex, NULL);

    // init semaphores
    sem_init(&client_ready, 0, 0);
    sem_init(&barber_ready, 0, 0);
    sem_init(&modifySeats, 0, 1);
    
    chairs = CHAIRS;
    total_clients = CLIENTS;
    available_chairs = CHAIRS; 
    
    /* Create barber thread*/
    int barber_result = pthread_create(&barber, NULL, (void *)barber_function, NULL);  
    if (barber_result) {
        perror("Failed to create thread.\n");
        exit(EXIT_FAILURE);
    } 
    
    /* Create customer_maker thread*/
    for (int i = 0; i < CLIENTS; i++) {
        int client_result = pthread_create(&clients[i], NULL, (void *)customer_function, &i);  
        if (client_result) {
            printf("Failed to create thread.\n");
            exit(EXIT_FAILURE);
        }
        usleep(100000);
    } 

    pthread_join(barber, NULL);

    for (int i = 0; i < CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
        
    printf("Number of customers that were forced to leave and got unserved: %d\n", left_and_unserved_clients);    	
}