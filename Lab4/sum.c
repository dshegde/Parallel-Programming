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

//#define N 1000

long N;

long compute(long i) { return sqrt(i*i); }

int main(int argc, char **argv) {
        struct timeval start1, end1;
        double sec, usec, msec;
        if (argc > 1)
        {
                N = atoi(argv[1]);
                if(N<=0)
                        return;
        }
        else
        {
                printf("Enter N value\n");
                return 1;
        }

  long long sum = 0;
  gettimeofday(&start1, NULL);
  for (long i = 0; i < N; i++)
    sum += compute(i);
gettimeofday(&end1, NULL);
usec = end1.tv_usec - start1.tv_usec;
msec = (usec/1000.0);
  printf("Sum of %ld sqrt(i*i) is %lld\n", N, sum);
  printf("Elapsed time (gettimeofday):  msec=%f ms\n", msec);
}