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
#include <thread>
#include <mutex>
#include <string>
#include "sys/time.h"
#include <omp.h>
#include <chrono>
using namespace std;
int N;
int P = 1;
int totalPrimes;
int* sieve;
bool* candidate;
mutex mtx;
mutex conMtx;

void ConsolePrint(string message)
{
    conMtx.lock();
    cout << message << "\n";
    conMtx.unlock();
}
void worker(int tID)
{
    int start, end;
    int remaining = N - sqrt(N);
    int div = (remaining) / P;
    start = (sqrt(N)) + (div * tID) + 1;
    if (tID  == (P-1))
    {
        end = N;
    }
    else
    {
        end = (sqrt(N)) + (div * (tID + 1));
    }
    for (int i = start; i <= end; i++)
    {
        candidate[i] = true;
    }
    for (int i = 0; i < sqrt(N); i++)
    {
        if (sieve[i] == -1)
            break;
        if (candidate[sieve[i]])
        {
                int temp = start;
                while (temp % sieve[i] != 0)
                {
                        temp++;
                }
            for (int j = temp; j <= end; j += sieve[i])
            {
                candidate[j] = false;
            }
        }
    }
for (int i = start; i <= end; i++)
    {
        if (candidate[i])
        {
            #pragma omp critical
            totalPrimes++;
        }
    }
}
int main(int argc, char** argv) {


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
ConsolePrint("prime ("+std::to_string(P)+" threads) over [2.." + std::to_string(N) + "] starting ...");
    int sqrtN = sqrt(N);
    candidate = new bool[N + 1];
    sieve = new int [sqrtN];

    int i,j;
    auto start = chrono::steady_clock::now();
    #pragma omp parallel for num_threads(P) shared(candidate) private(i)
    for (i = 2; i <= sqrtN; i++)
    {
        candidate[i] = true;
    }

    for (i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            #pragma omp parallel for num_threads(P) shared(candidate) private(j)
            for (j = i+i; j <= sqrtN; j += i)
            {
                candidate[j] = false;
            }
        }
    }
    int idx = 0;
    for (i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            sieve[idx] = i;
            idx++;
            totalPrimes++;
        }
    }

    for (i = 0; i < sqrtN; i++)
    {
        if (i+1 > totalPrimes)
        {
            sieve[i] = -1;
        }
    }
 #pragma omp parallel num_threads(P) shared(candidate, sieve, totalPrimes) private(i)
    #pragma omp single
    for (i = 0; i < P; i++)
    {
        #pragma omp task
        worker(i);
    }
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration<double, std::milli> (end - start);
    ConsolePrint("Elapsed time is: "+std::to_string(duration.count())+" ms");


    ConsolePrint("prime ("+std::to_string(P)+" threads) found " + std::to_string(totalPrimes) + " primes");
}