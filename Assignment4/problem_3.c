#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <sched.h>
 
 
int NUM_THREADS = 10;
int NUM_SECONDS;

volatile int in_cs =0;
//volatile clock_t start_time, end_time;
volatile int *entering;
volatile int *number;
struct timespec start_time, end_time;
long int ending;


void mfence (void) {
  asm volatile ("mfence" : : : "memory");
}
 
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  int count;
} thread_data_t; 

void lock(void *arg)
{
  int j;
  thread_data_t *data = (thread_data_t *)arg;
  int max = 0;

  /* memory barriers are necessary to prevent out of order execution. To force atomic execution multiple threads are executing,the following three mfence calls are added */
  entering[data->tid] = true;
  mfence();
  for (j=0; j<NUM_THREADS; j++)
    {
      if(max < number[j])
	max = number[j];
    }
  /* number and entering values need to modified in order.*/
  number[data->tid] = max + 1;
  mfence();
  entering[data->tid] = false;
  mfence();

  for(j=0; j<NUM_THREADS; j++)
    {
      while(entering[j]);
      while((number[j]!=0)&&((number[j]<number[data->tid])||((number[j]==number[data->tid])&&(j<data->tid))));
    }
}

void unlock(void *arg)
{
  thread_data_t *data = (thread_data_t *)arg;
  number[data->tid] = 0;
}

/* thread function */
void *thr_func(void *arg) 
{
  thread_data_t *data = (thread_data_t *)arg;
  data->count = 0;
clock_gettime(CLOCK_MONOTONIC, &end_time); 
  while(end_time.tv_sec < ending)
    {
clock_gettime(CLOCK_MONOTONIC, &end_time);
  lock(data);
  data->count += 1;
  assert (in_cs==0);
    in_cs++;
    assert (in_cs==1);
    in_cs++;
    assert (in_cs==2);
    in_cs++;
    assert (in_cs==3);
    in_cs=0;

  unlock(data);
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

//  start_time = clock();
//  end_time = start_time + (NUM_SECONDS * CLOCKS_PER_SEC);

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  ending = start_time.tv_sec + NUM_SECONDS;

  pthread_t thr[NUM_THREADS];
  int i, rc;
  /* create a thread_data_t argument array */
  thread_data_t thr_data[NUM_THREADS];

  entering = malloc(NUM_THREADS * sizeof(*entering)); 
  number = malloc(NUM_THREADS * sizeof(*number));

  for(i=0; i< NUM_THREADS; i++)
    {
      entering[i]=0;
      number[i]=0;
    }
 
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
