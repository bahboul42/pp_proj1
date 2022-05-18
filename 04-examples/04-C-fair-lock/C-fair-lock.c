#include <pthread.h>
#include <stdio.h>
#include <error.h>
#include <assert.h>
#include <unistd.h>
#define NTHREADS 4

pthread_mutex_t mutex;
pthread_cond_t cond;
unsigned pid_queue[NTHREADS];
unsigned pid_queue_first = 0, pid_queue_n = 0;
unsigned fair_lock_locked = 0;

void fair_lock(unsigned pid)
{
  pthread_mutex_lock(&mutex);
  if (pid_queue_n || fair_lock_locked)
    {
      pid_queue[(pid_queue_first + pid_queue_n) % NTHREADS]
	= pid;
      pid_queue_n++;
      while (fair_lock_locked || pid_queue[pid_queue_first] != pid)
	pthread_cond_wait(&cond, &mutex);
      pid_queue_first = (pid_queue_first + 1) % NTHREADS;
      pid_queue_n--;
    }
  fair_lock_locked = 1;
  pthread_mutex_unlock(&mutex);
}

void fair_unlock(void)
{
  pthread_mutex_lock(&mutex);
  fair_lock_locked = 0;
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);
}

void *mythread(void *arg) {
  unsigned id = * (unsigned *) arg;
  unsigned i = 30;
  while (--i) {
    fair_lock(id);
    printf("Thread %d entering critical section\n", id);
    usleep(10000);
    printf("Thread %d exiting critical section\n", id);
    fair_unlock();
  }
  return NULL;
}

int main(void) {
  unsigned i;
  pthread_t thread[NTHREADS];
  unsigned threadnb[NTHREADS];

  if (pthread_mutex_init(&mutex, NULL))
    error(-1, 0, "Error initializing mutex");

  if (pthread_cond_init(&cond, NULL))
    error(-1, 0, "Error initializing condition");

  for (i = 0; i < NTHREADS; i++) {
    threadnb[i] = i;
    if (pthread_create(&thread[i], NULL, mythread, &threadnb[i]))
      error(-1, 0, "Error creating thread");
    usleep(100);
  }
  for (i = 0; i < NTHREADS; i++)
    if (pthread_join(thread[i], NULL))
      error(-1, 0, "Error joining thread");

  if (pthread_mutex_destroy(&mutex))
    error(-1, 0, "Error destroying mutex");

  if (pthread_cond_destroy(&cond))
    error(-1, 0, "Error destroying condition");

  return 0;
}
