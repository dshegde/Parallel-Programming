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
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <map>
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
map<int, int> stats;
int idx = 0;
map<int, bool> progressMap;

void ConsolePrint(string message)
{
    conMtx.lock();
    cout << message << "\n";
    conMtx.unlock();
}
void worker(int tID)
{
    for (int i = 0; i < sieve.size(); i++)
    {
        unique_lock<mutex> lck(mtx);
        while (sieve[i] == 0)
        {
            cvar.wait(lck);
        }

        if (sieve[i] == -1)
        {
            //ConsolePrint("Breaking threadID: " + std::to_string(tID) + ". i: " + std::to_string(i));
            break;
        }

        if (progressMap.at(i))
        {
            //ConsolePrint("Skip threadID: " + std::to_string(tID) + ". i: " + std::to_string(i));
            continue;
        }

        progressMap.at(i) = true;
        if (candidate[sieve[i]])
        {
            int temp = sqrt(N) + 1;
            while (temp % sieve[i] != 0)
            {
                temp++;
            }

            stats.at(tID) = stats.at(tID) + 1;
            for (int j = temp; j <= N; j += sieve[i])
            {
                candidate[j] = false;
               /* ConsolePrint("threadID: " + std::to_string(tID) + ". i: " + std::to_string(i) + " j: " +
                std::to_string(j) );*/
            }
        }
        lck.unlock();
    }
}
int main(int argc, char** argv) {
    /*N = 10000;
    P = 30;
    thread th[30];*/

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

    for (int i = 2; i <= N; i++)
    {
        candidate[i] = true;
    }

    for (int i = 0; i <= sqrtN; i++)
    {
        progressMap.insert({ i, false });
    }

int idx = 0;
    for (int i = 2; i <= sqrt(N); i++)
    {
        if (candidate[i])
        {
            unique_lock<mutex> lck(mtx);
            sieve[idx] = i;
            idx++;
            totalPrimes++;
            lck.unlock();
            cvar.notify_all();
            for (int j = i + i; j <= sqrtN; j += i)
            {
                candidate[j] = false;
            }
        }
    }

    for (int i = 0; i < sqrtN; i++)
    {
        if (sieve[i] == 0)
        {
            sieve[i] = -1;
            cvar.notify_all();
        }
    }

    for (int i = 0; i < P; i++)
    {
        th[i].join();
    }

    for (int i = sqrtN + 1; i <= N; i++)
    {
        if (candidate[i])
        {
            totalPrimes++;
        }
    }
 auto end = chrono::steady_clock::now();
    auto duration = chrono::duration<double, std::milli> (end - start);
    ConsolePrint("Elapsed time is: "+std::to_string(duration.count())+" ms");

    ConsolePrint("prime ("+std::to_string(P)+") threads found " + std::to_string(totalPrimes) + " primes");
    cout<<"Worker Stats: [ ";
    for (int i = 0; i < P; i++)
    {
        cout<<std::to_string(stats[i])<<", ";
    }
    cout<<"]\n";
    /*for (int i = 0; i < P; i++)
    {
        ConsolePrint("thread id " + std::to_string(i) + " = " + std::to_string(stats[i]));
    }*/
}