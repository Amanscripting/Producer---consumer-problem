# Producer---consumer-problem using C
A thread-safe, bounded circular buffer shared between a producer thread
and a consumer thread, implemented in C using POSIX threads
# What is demonstrated
Multi-threading-Seprate producer and consumer threads running concurrently
Mutual-exclusion- a pthread_mutex_t protects the shared buffer, index pointers, and item count from race conditions
Condition variables — pthread_cond_t is used so threads block efficiently when the buffer is full or empty, instead of spinning in a loop
Circular buffer — fixed-size array reused via modulo arithmetic on the read/write indices
# Why I prefered using condition variables instea of just using mutex:
A mutex alone only prevents two threads from touching shared data at the
same time — it does not stop a consumer from trying to read an item
that hasn't been produced yet, or a producer from writing into a full
buffer. Condition variables let a thread sleep until a specific condition
becomes true (buffer no longer empty / no longer full), and the producer
and consumer signal each other when that happens. Which makes my program mmuch more efficent.
# Program:
```
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
```
# Output:
Produced: 1 (count=1)
Consumed: 1 (count=0)
Produced: 2 (count=1)
Produced: 3 (count=2)
Produced: 4 (count=3)
Produced: 5 (count=4)
Consumed: 2 (count=3)
Consumed: 3 (count=2)
Consumed: 4 (count=1)
Consumed: 5 (count=0)


