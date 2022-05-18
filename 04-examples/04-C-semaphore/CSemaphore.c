#include <pthread.h>
#include <stdio.h>
#include <error.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#define NTHREADS 4

sem_t mysemaphore;

void *mythread(void *arg) {
  unsigned id = * (unsigned *) arg;
  unsigned i = 30;
  while (--i) {
    sleep(1);
    sem_wait(&mysemaphore);
    printf("Thread %d entering critical section\n", id);
    sleep(1);
    printf("Thread %d exiting critical section\n", id);
    sem_post(&mysemaphore);
  }
  return NULL;
}

int main(void) {
  unsigned i;
  pthread_t thread[NTHREADS];
  unsigned threadnb[NTHREADS];

  if (sem_init(&mysemaphore, 0, 2))
    error(-1, 0, "Error initializing semaphore");
  
  for (i = 0; i < NTHREADS; i++) {
    threadnb[i] = i;
    if (pthread_create(&thread[i], NULL, mythread, &threadnb[i]))
      error(-1, 0, "Error creating thread");
  }
  for (i = 0; i < NTHREADS; i++)
    if (pthread_join(thread[i], NULL))
      error(-1, 0, "Error joining thread");

  if (sem_destroy(&mysemaphore))
    error(-1, 0, "Error destroying semaphore");
  return 0;
}
