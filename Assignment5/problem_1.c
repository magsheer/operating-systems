#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<pthread.h>
#include<assert.h>
#include<time.h>
#include<unistd.h>

long n_cats=0;
long n_dogs=0;
long n_birds=0;
volatile  long pl_cats=0, pl_dogs=0, pl_birds=0;
volatile  long cats=0, dogs=0, birds=0;
volatile  int stop=1;

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cats = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_dogs = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_birds = PTHREAD_COND_INITIALIZER;

void play(void) 
{
  int i;
  for (i=0; i<10; i++) {
    assert(cats >= 0 && cats <= n_cats);
    assert(dogs >= 0 && dogs <= n_dogs);
    assert(birds >= 0 && birds <= n_birds);
    assert(cats == 0 || dogs == 0);
    assert(cats == 0 || birds == 0);
  }
}

void cat_enter(void)
{
  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {
      fprintf(stderr, "\nThread mutex lock failure\n");
      exit(1);
    }

  while(dogs>0 || birds>0)
    pthread_cond_wait(&c_cats, &mutex_lock);
      
  pl_cats++;
  cats++;
  play();

  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      fprintf(stderr, "\nThread mutex unlock failure\n");
      exit(1);
    }
}

void cat_exit(void)
{

  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {    
      fprintf(stderr, "\nThread mutex lock failure\n");
            exit(1);
    }


     cats--;
   
   
     pthread_cond_broadcast(&c_birds);
     pthread_cond_broadcast(&c_dogs);
   
  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      
      fprintf(stderr, "\nThread mutex unlock failure\n");
           exit(1); 
    }
 }

void bird_enter(void)
{
  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {
      
      fprintf(stderr, "\nThread mutex lock failure\n");
            exit(1);
    }

  while(cats!=0)
      pthread_cond_wait(&c_birds, &mutex_lock);
    
  pl_birds++;
  birds++;
  play();

  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      fprintf(stderr, "\nThread mutex unlock failure\n");
            exit(1);
    }
}


void bird_exit(void)
{

  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {    
      fprintf(stderr, "\nThread mutex lock failure\n");
            exit(1);
    }


     birds--;
   

     pthread_cond_broadcast(&c_cats);

  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      
      fprintf(stderr, "\nThread mutex unlock failure\n");
           exit(1);
    }
 }

void dog_enter(void)
{
  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {
      
      fprintf(stderr, "\nThread mutex lock failure\n");
            exit(1);
    }

  while(cats!=0)
      pthread_cond_wait(&c_dogs, &mutex_lock);
    
  pl_dogs++;
  dogs++;
  play();

  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      fprintf(stderr, "\nThread mutex unlock failure\n");
            exit(1);
    }
}


void dog_exit(void)
{

  int result = pthread_mutex_lock(&mutex_lock);
  if(result!=0)
    {    
      fprintf(stderr, "\nThread mutex lock failure\n");
            exit(1);
    }


     dogs--;
   
   
     pthread_cond_signal(&c_cats);

  result = pthread_mutex_unlock(&mutex_lock);
  if(result!=0)
    {
      
      fprintf(stderr, "\nThread mutex unlock failure\n");
            exit(1);
    }
 }


 void *cat(void *arg)
 {
     while(stop)
     {
       cat_enter();
       cat_exit();
     }
     return EXIT_SUCCESS;      
 }

void *bird(void *arg)
 {
   while(stop)
     {
       bird_enter();
       bird_exit();
     }
     return EXIT_SUCCESS;
 }

void *dog(void *arg)
 {
   while(stop)
     {
       dog_enter();
       dog_exit();
     }
   return EXIT_SUCCESS;
 }


int main(int argc, char **argv)
{

  //cmd line arguments check
  if(argc != 4)
    {
      fprintf(stderr, "\nEnter the number of cats, dogs and birds in the pet motel (three values)\n");
      return EXIT_FAILURE;
    }
  
  //printf("here");
  //validity check
  if(!isdigit(*argv[1]) || !isdigit(*argv[2]) || !isdigit(*argv[3]))
    {
      fprintf(stderr,"\nInputs should positive number\n");
      return EXIT_FAILURE;
    }

  n_cats = atoi(argv[1]);
  n_dogs = atoi(argv[2]);
  n_birds = atoi(argv[3]);
  printf("%ld %ld %ld",n_cats,n_dogs,n_birds);

  //0-99 range check
  if(n_cats<0 || n_cats>99 || n_dogs<0 ||  n_dogs>99 || n_birds<0 ||n_birds>99)
    {
      fprintf(stderr,"\nInputs should be between 0-99\n");
      return EXIT_FAILURE;      
    }

  //create threads
   int count = n_cats + n_dogs + n_birds;
  pthread_t *animal_threads = (pthread_t *) malloc(count*sizeof(pthread_t));
   int c,d,b;
  
  for(c=0;c<n_cats;c++)
    pthread_create(&animal_threads[c],NULL,cat,NULL);

  for(d=0;d<n_dogs;d++)
  {
    c = n_cats+d;
    pthread_create(&animal_threads[c],NULL,dog,NULL);
  }

  for(b=0;b<n_birds;b++)
  {
    c = n_cats+n_dogs+b;
    pthread_create(&animal_threads[c],NULL,bird,NULL);
  }

  sleep(10);
  stop = 0;

  for (c=0;c<count;c++)
    pthread_join(animal_threads[c],NULL);

  printf("\ncat play = %ld, dog play = %ld, bird play = %ld\n", pl_cats, pl_dogs, pl_birds);

  return EXIT_SUCCESS;
}
