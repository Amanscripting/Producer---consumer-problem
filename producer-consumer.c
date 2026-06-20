#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 5

int buffer[SIZE];
int count = 0;
int in = 0;
int out = 0;

pthread_mutex_t lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

void produce(int item) {
    pthread_mutex_lock(&lock);

    while (count == SIZE) {
        pthread_cond_wait(&not_full, &lock);
    }

    buffer[in] = item;
    in = (in + 1) %SIZE;
    count++;
    printf("Produced: %d (count=%d)\n", item, count);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&lock);
}

int consume() {
    pthread_mutex_lock(&lock);

    while (count == 0) {
        pthread_cond_wait(&not_empty, &lock);
    }

    int item = buffer[out];
    out = (out + 1) % SIZE;
    count--;
    printf("Consumed: %d (count=%d)\n", item, count);

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&lock);
    return item;
}

void* producer_thread(void* arg) {
    for (int i = 1; i <= 5; i++) produce(i);
    return NULL;
}

void* consumer_thread(void* arg) {
    for (int i = 1; i <= 5; i++) consume();
    return NULL;
}

int main() {
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer_thread, NULL);
    pthread_create(&cons, NULL, consumer_thread, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    return 0;
}
