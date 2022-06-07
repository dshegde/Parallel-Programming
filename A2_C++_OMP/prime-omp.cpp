//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// A prime-finding program (Sequential version).
//
// Usage:
//   linux> ./prime N
//
#include <iostream>
#include <cmath>
#include <chrono>
#include "unistd.h"
#include <omp.h>
using namespace std;

int main(int argc, char **argv) {
  int N,P;


  if (argc < 2) {
    cout << "Usage: ./prime N\n";
    exit(0);
  }

  if (argc == 2)
  {
          P = 1;
          if ((N = atoi(argv[1])) < 2) {
                 cout << "N must be greater than 1\n";
                exit(0);
                }
  }
  if (argc > 2)
  {
          if ((N = atoi(argv[1])) < 2) {
                 cout << "N must be greater than 1\n";
                exit(0);
                }
          if ((P = atoi(argv[2])) <=0 )
                          {
                                  cout << "P must be greater than 0\n";
                          exit(0);
                          }
  }

  cout << "prime ("<<P<<" threads) over [2.." << N << "] starting ...\n";
auto start = chrono::steady_clock::now();

  bool candidate[N+1];
  int i1;
#pragma omp parallel for num_threads(P) shared(candidate) private(i1)
  for (i1 = 2; i1 <= N; i1++)
    candidate[i1] = true;
//#pragma omp barrier
  int sqrtN = sqrt(N);
  int j;

  for (int i = 2; i <= sqrt(N); i++)
    if (candidate[i])
            #pragma omp parallel for num_threads(P) shared(candidate) private(j)
      for (j = i+i; j <= N; j += i)
        candidate[j] = false;

//#pragma omp barrier
  int totalPrimes = 0;
  int i2;
  #pragma omp parallel for num_threads(P) shared(totalPrimes,candidate) private(i2)
  for (i2 = 2; i2 <= N; i2++)
    if (candidate[i2])
        #pragma omp critical
      totalPrimes++;

  auto end = chrono::steady_clock::now();
  auto duration = chrono::duration<double, std::milli> (end - start);

  cout << "prime ("<<P<<" threads) found " << totalPrimes << " primes in "<< duration.count() << " ms\n";
}