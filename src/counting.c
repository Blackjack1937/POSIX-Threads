#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define N_ITER 1000000

int result;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

void *count(void *arg)
{
  int i = 0;
  for (i = 0; i < N_ITER; i++)
  {
    pthread_mutex_lock(&count_lock);
    result = result + 1;
    pthread_mutex_unlock(&count_lock);
  }
  return (void *)0;
}

int main(int argc, char **argv)
{

  int i;
  int nb_threads = 0;
  pthread_t *tids;

  result = 0;

  if (argc != 2)
  {
    fprintf(stderr, "usage : %s nb_threads\n", argv[0]);
    exit(-1);
  }

  nb_threads = atoi(argv[1]);
  tids = malloc(nb_threads * sizeof(pthread_t));

  for (i = 0; i < nb_threads; i++)
  {
    if (pthread_create(&tids[i], NULL, count, NULL) != 0)
    {
      fprintf(stderr, "Failed to create thread number %d\n", i);
    }
  }

  /* Wait until every thread ended */
  for (i = 0; i < nb_threads; i++)
  {
    pthread_join(tids[i], NULL);
  }

  free(tids);

  printf("the result of the computation is %d\n", result);

  return EXIT_SUCCESS;
}
