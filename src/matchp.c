#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    char *lyrics;
    int repeat_count;
} supporter_args_t;

void *supporter(void *arg)
{
    supporter_args_t *args = (supporter_args_t *)arg;
    char *str = args->lyrics;
    int repeat = args->repeat_count;
    int i;
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    for (i = 0; i < repeat; i++)
    {
        printf("Process %d Thread %lx: %s\n", pid, (unsigned long)tid, str);
    }

    /* Free the allocated memory */
    free(args->lyrics);
    free(args);

    return NULL;
}

int main(int argc, char **argv)
{
    int team1_count, team1_repeats;
    int team2_count, team2_repeats;
    int i;
    int nb_threads = 0;
    pthread_t *tids;

    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s team1_count team1_repeats team2_count team2_repeats\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    team1_count = atoi(argv[1]);
    team1_repeats = atoi(argv[2]);
    team2_count = atoi(argv[3]);
    team2_repeats = atoi(argv[4]);

    nb_threads = team1_count + team2_count;
    tids = malloc(nb_threads * sizeof(pthread_t));
    if (tids == NULL)
    {
        perror("Failed to allocate memory for thread IDs");
        exit(EXIT_FAILURE);
    }

    /* Create threads for team1 (French supporters) */
    for (i = 0; i < team1_count; i++)
    {
        supporter_args_t *args = malloc(sizeof(supporter_args_t));
        if (args == NULL)
        {
            perror("Failed to allocate memory for supporter arguments");
            exit(EXIT_FAILURE);
        }
        args->lyrics = strdup("Allons enfants de la patrie");
        if (args->lyrics == NULL)
        {
            perror("Failed to duplicate lyrics string");
            free(args);
            exit(EXIT_FAILURE);
        }
        args->repeat_count = team1_repeats;

        if (pthread_create(&tids[i], NULL, supporter, args) != 0)
        {
            perror("Failed to create thread");
            free(args->lyrics);
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    /* Create threads for team2 (English supporters) */
    for (; i < nb_threads; i++)
    {
        supporter_args_t *args = malloc(sizeof(supporter_args_t));
        if (args == NULL)
        {
            perror("Failed to allocate memory for supporter arguments");
            exit(EXIT_FAILURE);
        }
        args->lyrics = strdup("Swing low, sweet chariot");
        if (args->lyrics == NULL)
        {
            perror("Failed to duplicate lyrics string");
            free(args);
            exit(EXIT_FAILURE);
        }
        args->repeat_count = team2_repeats;

        if (pthread_create(&tids[i], NULL, supporter, args) != 0)
        {
            perror("Failed to create thread");
            free(args->lyrics);
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for all threads to finish */
    for (i = 0; i < nb_threads; i++)
    {
        if (pthread_join(tids[i], NULL) != 0)
        {
            perror("Failed to join thread");
            /* Continue to join the rest of the threads */
        }
    }

    free(tids);
    return EXIT_SUCCESS;
}
