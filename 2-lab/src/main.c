#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE 5000000
#define NUM_THREADS 16

int array[ARRAY_SIZE];
int min = 99999999;
int max = -100;

pthread_mutex_t mutex_min;
pthread_mutex_t mutex_max;

void *find_min_max(void *arg) {
    int thread_id = *((int *)arg);
    int chunk_size = ARRAY_SIZE / NUM_THREADS;
    
    int start = thread_id * chunk_size;
    int end = (thread_id + 1) * chunk_size;
    
    int local_min = array[start];
    int local_max = array[start];
    
    for (int i = start; i < end; i++) {
        if (array[i] < local_min) {
            local_min = array[i];
        }
        if (array[i] > local_max) {
            local_max = array[i];
        }
    }
    
    pthread_mutex_lock(&mutex_min);
    if (local_min < min) {
        min = local_min;
    }
    pthread_mutex_unlock(&mutex_min);
    
    pthread_mutex_lock(&mutex_max);
    if (local_max > max) {
        max = local_max;
    }
    pthread_mutex_unlock(&mutex_max);
    
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    pthread_mutex_init(&mutex_min, NULL);
    pthread_mutex_init(&mutex_max, NULL);
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 10000000;
    }
    
    clock_t start_time = clock();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, find_min_max, &thread_ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Number of threads: %d\n", NUM_THREADS);
    printf("Minimum element: %d\n", min);
    printf("Maximum element: %d\n", max);
    printf("Execution time: %f seconds\n", execution_time);
    
    pthread_mutex_destroy(&mutex_min);
    pthread_mutex_destroy(&mutex_max);
    
    return 0;
}
