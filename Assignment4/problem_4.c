#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
 
int NUM_THREADS = 10;
int NUM_SECONDS;
volatile int in_cs =0;
//volatile clock_t start_time, end_time;
struct timespec start_time,end_time;
long int ending;

/*
 * atomic_cmpxchg
 * 
 * equivalent to atomic execution of this code:
 *
 * if (*ptr == old) {
 *   *ptr = new;
 *   return old;
 * } else {
 *   return *ptr;
 * }
 *
 */
static inline int atomic_cmpxchg (volatile int *ptr, int old, int new)
{
  int ret;
  asm volatile ("lock cmpxchgl %2,%1"
    : "=a" (ret), "+m" (*ptr)     
    : "r" (new), "0" (old)      
    : "memory");         
  return ret;                            
}

struct spin_lock_t
{
  volatile int flag;
};

struct spin_lock_t some_lock;

void spin_lock (struct spin_lock_t *s)
{
  while(atomic_cmpxchg(&s->flag,0,1));
}

void spin_unlock (struct spin_lock_t *s)
{
  s->flag = 0;
}
 
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  int count;
} thread_data_t; 


/* thread function */
void *thr_func(void *arg) 
{
  thread_data_t *data = (thread_data_t *)arg;
  data->count = 0;
 
  while(end_time.tv_sec < ending)
    {
      clock_gettime(CLOCK_MONOTONIC, &end_time);
      spin_lock(&some_lock);
  data->count += 1;
  assert (in_cs==0);
    in_cs++;
    assert (in_cs==1);
    in_cs++;
    assert (in_cs==2);
    in_cs++;
    assert (in_cs==3);
    in_cs=0;

  spin_unlock(&some_lock);
    }
  printf("thread id: %d, entered critical section %d times\n", data->tid, data->count);
 
  pthread_exit(NULL);
}
 
int main(int argc, char **argv) 
{

  if(argc!=3)
    {
      fprintf(stderr, "\nEnter number of threads and number of seconds to run for");
      return -1;
    }

  NUM_THREADS = atoi(argv[1]);
  NUM_SECONDS = atoi(argv[2]);

  //start_time = clock();
  //end_time = start_time + (NUM_SECONDS * CLOCKS_PER_SEC);

 clock_gettime(CLOCK_MONOTONIC, &start_time);
  ending = start_time.tv_sec + NUM_SECONDS;


  pthread_t thr[NUM_THREADS];
  int i, rc;

  /* create a thread_data_t argument array */
  thread_data_t thr_data[NUM_THREADS];
 
  /* create threads */
  for (i = 0; i < NUM_THREADS; ++i) {
    thr_data[i].tid = i;
    if ((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      return EXIT_FAILURE;
    }
  }
  /* block until all threads complete */
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_join(thr[i], NULL);
  }
 
  return EXIT_SUCCESS;
}
