//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// The sum program (Pthreads version).
//
//
#include <stdio.h>
#include <pthread.h>

#define N 1000 	/* problem parameter */
#define P 10 	/* number of threads */

int sum = 0; 
pthread_mutex_t mtx;   

int compute(int i) { return i*i; }

void worker(long tid) {
  int low = (N/P) * tid; 	/* a simplistic partition scheme */
  int high = low + (N/P);
  int psum = 0;
  for (int i = low; i < high; i++)
    psum += compute(i);
  pthread_mutex_lock(&mtx);
  sum += psum;
  pthread_mutex_unlock(&mtx);
} 

int main(int argc, char **argv) {
  pthread_t thread[P];
  pthread_mutex_init(&mtx, NULL);     /* initialize mutex */
  for (long k = 0; k < P; k++)        /* create threads */
    pthread_create(&thread[k], NULL, (void*)worker, (void*)k);
  for (long k = 0; k < P; k++)        /* join threads */
    pthread_join(thread[k], NULL);
  printf("The sum is %d (should be 332833500)\n", sum);
}  
