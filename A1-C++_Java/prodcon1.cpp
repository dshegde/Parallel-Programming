#include <thread>
#include <mutex>
#include <condition_variable>
#include "queue.h"
#include <iostream>
#define _GNU_SOURCE
#include <sched.h>

using namespace std;
mutex mtx;
condition_variable cvar_prod;
condition_variable cvar_cons;
int BUFSIZE = 20; // queue capacity
int NUMITEMS = 100; // total number of data items

Queue q(BUFSIZE);

void Producer()
{
        cout<<"Producer starting on core "<<sched_getcpu()<<"\n";
        for (int i=0; i<NUMITEMS; i++)
        {
                unique_lock<mutex> lck(mtx);
                while (q.isFull())
                {
                        cvar_prod.wait(lck, [](){ return !(q.isFull());});
                }

                q.add(i);
                cout<<"Producer added  "<<i<<"  (qsz : "<<q.size()<<")\n";
                lck.unlock();
                cvar_cons.notify_one();
        }
        cout<<"Producer ending\n";
}
void Consumer()
{
        //cout<<"Running on thread "<<pthread_self()<<"\n";
        cout<<"Consumer starting on core "<<sched_getcpu()<<"\n";
        for (int i=0; i<NUMITEMS; i++)
        {
                unique_lock<mutex> lck(mtx);
                while (q.isEmpty())
                {
                        cvar_cons.wait(lck, [](){ return !(q.isEmpty());});
                }
                int value = q.remove();
                cout<<"Consumer removed  "<<value<<"  (qsz : "<<q.size()<<")\n";
                lck.unlock();
                cvar_prod.notify_one();
        }
        cout<<"Consumer ending\n";
}
int main(int argc, char **argv)
{
        //Queue q(BUFSIZE);
        thread thd1(Consumer);
        thread thd2(Producer);
        thd1.join();
        thd2.join();
        cout<<"Main: all done!\n";
        return 1;
}