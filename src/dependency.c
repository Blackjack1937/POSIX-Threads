#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[NUM_THREADS];

int finished[NUM_THREADS] = {0};

// Helper function to simulate work with random sleep
void random_sleep()
{
    sleep(rand() % 3 + 1); // Sleep for 1-3 seconds
}

// Thread function
void *thread_function(void *arg)
{
    int tid = *(int *)arg;
    printf("Thread %d: starting.\n", tid);

    random_sleep();

    // Mark thread as finished and signal any dependent threads
    pthread_mutex_lock(&mutex);
    finished[tid] = 1;
    pthread_cond_broadcast(&cond[tid]);
    pthread_mutex_unlock(&mutex);

    printf("Thread %d: finishing.\n", tid);
    free(arg);
    return NULL;
}

// Thread creation and dependency handling
void create_thread(pthread_t *thread, int tid, int *dependencies, int num_deps)
{
    // Wait for dependencies to finish
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_deps; i++)
    {
        while (!finished[dependencies[i]])
        {
            pthread_cond_wait(&cond[dependencies[i]], &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);

    int *tid_arg = malloc(sizeof(int));
    *tid_arg = tid;
    pthread_create(thread, NULL, thread_function, tid_arg);
}

int main()
{
    pthread_t threads[NUM_THREADS];
    srand(time(NULL));

    // Initialize condition variables
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_cond_init(&cond[i], NULL);
    }

    // Create threads with dependencies
    create_thread(&threads[0], 0, NULL, 0);
    create_thread(&threads[1], 1, (int[]){0}, 1);
    create_thread(&threads[2], 2, (int[]){0}, 1);
    create_thread(&threads[3], 3, (int[]){1, 2}, 2);
    create_thread(&threads[4], 4, (int[]){1, 2}, 2);
    create_thread(&threads[5], 5, (int[]){1, 2}, 2);
    create_thread(&threads[6], 6, (int[]){3, 4}, 2);
    create_thread(&threads[7], 7, (int[]){3, 4}, 2);
    create_thread(&threads[8], 8, (int[]){5}, 1);
    create_thread(&threads[9], 9, (int[]){6, 7, 8}, 3);

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_cond_destroy(&cond[i]);
    }
    pthread_mutex_destroy(&mutex);

    printf("All threads completed.\n");
    return 0;
}
