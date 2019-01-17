#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

/*MONTE CARLO method to calculate value of PI*/

pthread_mutex_t some_lock;
volatile int points_circle=0;
volatile int points_total=0;

 
int NUM_THREADS = 0;
int NUM_SECONDS;
volatile clock_t start_time, end_time;

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
 
  while(clock() < end_time)
    {
      pthread_mutex_lock(&some_lock);
      
      float a,b;
      a=((float)rand()/RAND_MAX)*2-1;
      b=((float)rand()/RAND_MAX)*2-1;

      points_total++;
      float distance = sqrt(a*a+b*b);
      if(distance <= 1)
	points_circle++;

      pthread_mutex_unlock(&some_lock);     
    }
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

  start_time = clock();
  end_time = start_time + (NUM_SECONDS * CLOCKS_PER_SEC);

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

  float pi_ = 4* (float)points_circle /(float)points_total;

  printf("\nEstimated value of PI:%f \n \n",pi_);
 
  return EXIT_SUCCESS;
}
