//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// The sum program (Sequential version).
//
//
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "sys/time.h"
#include <omp.h>
//#define N 1000

long N;

long compute(long i) { return sqrt(i*i); }

int main(int argc, char **argv) {
        int P;
        struct timeval start1, end1;
        double sec, usec, msec;
        if (argc == 2)
        {
                N = atoi(argv[1]);
                P = 1;
                if(N<=0)
                        return 1;
        }
        else if (argc == 3)
        {
                N = atoi(argv[1]);
                P = atoi(argv[2]);
                if (P <= 0 || N<= 0)
                        return 1;
        }
        else
        {
                printf("Enter N value\n");
                return 1;
        }

  long sum = 0;
  long i;
  int tid;
  gettimeofday(&start1, NULL);
#pragma omp parallel for num_threads(P) reduction(+:sum)  private(i)
  for (i = 0; i < N; i++)
  {
          //tid = omp_get_thread_num();
          //printf("Thread %d running..\n",tid);
    sum += compute(i);
  }
gettimeofday(&end1, NULL);
usec = end1.tv_usec - start1.tv_usec;
msec = usec/1000;
  printf("Sum of %ld sqrt(i*i) is %ld\n", N, sum);

  printf("Elapsed time (gettimeofday): msec=%f ms\n",msec);
}