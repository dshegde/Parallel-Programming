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
#include <chrono>
#include <vector>
#include <map>
#include <shared_mutex>
#include <condition_variable>
using namespace std;
int N;
int P;
int totalPrimes;
vector<int> sieve;
bool* candidate;
mutex conMtx;
mutex mtx;
condition_variable cvar;
int totalSieves = 0;
map<int, int> stats; //map to record thread stats
bool allFound = false; //flag to check if all primes found under sqrt(N)

void ConsolePrint(string message)
{
    conMtx.lock();
    cout << message << "\n";
    conMtx.unlock();
}
void worker(int tID) {
    int start, end;
    int remaining = N - sqrt(N);
    int div = (remaining) / P;

    start = (sqrt(N)) + (div * tID) + 1;

    if (tID == (P - 1))
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
    for (int i = 0; i < sieve.size(); i++) {

        unique_lock<mutex> lck(mtx);
        while (sieve[i] == 0 && !allFound)
        {
            cvar.wait(lck);
        }
        lck.unlock();

        if (i >= totalSieves)
        {
            break;
        }
if (candidate[sieve[i]])
        {
            int temp = start;
            while (temp % sieve[i] != 0)
            {
                temp++;
            }

            //ConsolePrint(
            //    "starting threadID: " + std::to_string(tID) + ". Start: " + std::to_string(temp) + " End: " +
            //    std::to_string(end) + " Sieve: " + std::to_string(sieve[i]));

            for (int j = temp; j <= end; j += sieve[i])
            {
                candidate[j] = false;
                // ConsolePrint("threadID: " + std::to_string(tID) + ". i: " + std::to_string(i) + " j: " +
                //                std::to_string(j) );
            }
        }
    }

    for (int i = start; i <= end; i++)
    {
        if (candidate[i])
        {
            unique_lock<mutex> lck(mtx);
            totalPrimes++;
            lck.unlock();
        }
    }
}
int main(int argc, char** argv) {

    /*N = 10000;
    P = 50;
    thread th[50];*/

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
int sqrtN = sqrt(N);
    ConsolePrint("prime (" +std::to_string(P)+" threads) over [2.." + std::to_string(N) + "] starting ...");
    candidate = new bool[N + 1];
    sieve.resize(sqrtN);

    for (int i = 0; i < P; i++)
    {
        stats.insert({ i, 0 });
    }

auto start = chrono::steady_clock::now();

    thread th[P];
    for (int i = 0; i < P; i++)
    {
        th[i] = thread(worker, i);
    }

    for (int i = 2; i <= sqrtN; i++)
    {
        candidate[i] = true;
    }

    int idx = 0;
    for (int i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            totalSieves += 1;
            unique_lock<mutex> lck(mtx);
            sieve[idx] = i;
            idx++;
            lck.unlock();
            cvar.notify_all();
            for (int j = i + i; j <= sqrtN; j += i)
            {
                candidate[j] = false;
            }
        }
    }
 allFound = true;
    cvar.notify_all();

    for (int i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            unique_lock<mutex> lck(mtx);
            totalPrimes++;
            lck.unlock();
        }
    }

    for (int i = 0; i < P; i++)
    {
        th[i].join();
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration<double, std::milli> (end - start);
    ConsolePrint("Elapsed time is: "+std::to_string(duration.count())+" ms");
    ConsolePrint("prime ("+std::to_string(P)+" threads) found " + std::to_string(totalPrimes) + " primes");
}