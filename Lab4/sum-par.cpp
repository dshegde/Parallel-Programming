//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// The sum program (Sequential version).
//
//
#include <string>
#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
using namespace std;

int N;
int P;
mutex mtx;
atomic<long long> sum(0);

long compute(long i)
{
        return sqrt(i*i);
}

void worker(long tid) {
  long low = (N/P) * tid;      // a simplistic partition scheme
  long high = low + (N/P);
  long long psum = 0;
  mtx.lock();
  //cout<<"Thread "<<tid<<" running on range ["<<low<<".."<<high<<"] \n";
  mtx.unlock();
  for (long i = low; i < high; i++)
    psum += compute(i);
  //mtx.lock();
  sum += psum;
  //mtx.unlock();
}
int main(int argc, char **argv) {


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


 thread t[P];
 auto start = chrono::steady_clock::now();
  for (long k = 0; k < P; k++)      // create threads
    t[k] = thread(worker, k);
  auto end = chrono::steady_clock::now();
  for (int k = 0; k < P; k++)      // join threads
    t[k].join();


  cout<<"Sum of "<<N<<" sqrt(i*i) is "<<sum<<"\n";
  auto duration = chrono::duration<double, std::milli> (end - start);
cout << "Elapsed time: " << duration.count() << " ms\n";

}