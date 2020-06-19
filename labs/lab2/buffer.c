#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

typedef unsigned int buffer_item;

buffer_item buffer[BUFFER_SIZE]; /* shared data buffer */

/* Add declaration of pthread mutex lock here; it is a shared data item */
/* See the lab instructions to find out how to initialize the lock, */
/* and also how to acquire the lock before entering a critical section, */
/* and how to release the lock after exiting a critical section. */
pthread_mutex_t p_mutex;
pthread_mutex_t c_mutex;

sem_t empty;    /* shared data empty counting semaphore */
sem_t full; /* shared data full counting semaphore */
int in = 0; /* shared data to be used by producers for next position in */
        /* buffer to insert produced item */
int out = 0;    /* shared data to be used by consumers for next position in */
        /* buffer from which to consume item */
unsigned int seed = 100;    /* seed value for rand_r; initialize to 100 */
unsigned int *seedp = &seed;    /* seed pointer to seed for rand_r(), a  */
                /* re-entrant, and therefore thread-safe, */
                /* version of rand() */

void insert_item (buffer_item);
void remove_item (buffer_item *);
void *producer (void *);
void *consumer (void *);

/* function shared by all producer threads */
void *producer (void *param) {
    buffer_item rand;
    while (1) {
        /* generate a random number between 0 and 99 */
        rand = rand_r(seedp) % 100;
        /* sleep for rand * 10000 microseconds */
        usleep(rand * 10000);
        /* insert item into buffer */
        insert_item(rand);
    }
}

/* function shared by all consumer threads */
void *consumer (void *param) {
    buffer_item rand;
    while (1) {
        /* generate a random number between 0 and 99 */
        rand = rand_r(seedp) % 100;
        /* sleep for rand * 10000 microseconds */
        usleep(rand * 10000);
        /* insert item into buffer */
        remove_item(&rand);
    }
}

/* function shared by all producer threads */
void insert_item(buffer_item item) {
    /* fill in code to insert an item into buffer */
    sem_wait(&empty);
    pthread_mutex_lock(&p_mutex);
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    printf("producer produced %d\n", item);
    pthread_mutex_unlock(&p_mutex);
    sem_post(&full);
}

/* function shared by all consumer threads */
void remove_item(buffer_item *item) {
    /* fill in code to remove an object from buffer and place it in *item */
    sem_wait(&full);
    pthread_mutex_lock(&c_mutex);
    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    printf("\tconsumer consumed %d\n", *item);
    pthread_mutex_unlock(&c_mutex);
    sem_post(&empty);
}

int main(int argc, char *argv[]) {
    int sleep_time = atoi(argv[1]);
    int num_prod_threads = atoi(argv[2]);
    int num_cons_threads = atoi(argv[3]);
    int i;

    pthread_attr_t attr;
    pthread_t prod_thread_id[num_prod_threads];
    pthread_t cons_thread_id[num_cons_threads];

    /* initialize mutex locks using pthread_mutex_init */
    pthread_mutex_init(&p_mutex, NULL);
    pthread_mutex_init(&c_mutex, NULL);

    /* initialize full semaphore using sem_init */
    sem_init(&full, 0, 0);

    /* initialize empty semaphore using sem_init */
    sem_init(&empty, 0, BUFFER_SIZE);

    /* initialize all buffer_items to 0 */
    for (i = 0; i < BUFFER_SIZE; ++i) {
        buffer[i] = 0;
    }

    /* create producer threads */
    pthread_attr_init(&attr);
    for (i = 0; i < num_prod_threads; i++) {
        pthread_create(&prod_thread_id[i], &attr, producer, NULL);
    }

    /* create consumer threads */
    pthread_attr_init(&attr);
    for (i = 0; i < num_cons_threads; i++) {
        pthread_create(&cons_thread_id[i], &attr, consumer, NULL);
    }
    /* sleep for number of seconds equal to second param on command line */
    sleep(sleep_time);

    return 0;
}
