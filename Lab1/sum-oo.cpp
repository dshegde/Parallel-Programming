//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// The sum program (C++11 mutex version).
//
//
#include <iostream> 
#include <thread> 
#include <mutex> 
using namespace std; 

#define N 1000 	// problem parameter
#define P 10 	// number of threads

int sum = 0; 
mutex mtx;   

int compute(int i) { return i*i; }

void worker(int tid) {
  int low = (N/P) * tid;      // a simplistic partition scheme
  int high = low + (N/P);
  int psum = 0;
  for (int i = low; i < high; i++)
    psum += compute(i);
  mtx.lock();
  sum += psum;
  mtx.unlock();
} 

int main(int argc, char **argv) {
  thread thd[P];
  for (int k = 0; k < P; k++)      // create threads
    thd[k] = thread(worker, k);
  for (int k = 0; k < P; k++)      // join threads
    thd[k].join();
  cout << "The sum is " << sum << " (should be 332833500)\n";
}  
