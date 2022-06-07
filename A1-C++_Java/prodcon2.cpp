#include <thread>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sched.h>
#define _GNU_SOURCE
#include "queue.h"

using namespace std;
mutex mtx;
mutex mtxConsolePrint;
condition_variable cvar;
int BUFSIZE = 20; // queue capacity
int NUMITEMS = 100; // total number of data items
map<int, int> consStat;
int numCons;
Queue q(BUFSIZE);
int partitionCons;

void ConsolePrint(string message)
{
    mtxConsolePrint.lock();
    cout << message << "\n";
    mtxConsolePrint.unlock();
}
void Producer()
{
    ConsolePrint("Producer starting on core " + std::to_string(sched_getcpu()));
    for (int i = 0; i < NUMITEMS; i++)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isFull())
        {
            cvar.wait(lck);
        }

        q.add(i);
        ConsolePrint("Producer added  " + std::to_string(i) + "  (qsz : " + std::to_string(q.size()) + ")");
        lck.unlock();
        cvar.notify_all();
    }

    for (int i=0; i<numCons; i++)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isFull())
        {
            cvar.wait(lck);
        }

        q.add(-1);
        ConsolePrint("Producer added  " + std::to_string(i) + "  (qsz : " + std::to_string(q.size()) + ")");
        lck.unlock();
        cvar.notify_all();
    }

    //ConsolePrint("Producer ending");
}
void Consumer(int k)
{
    ConsolePrint("Consumer ["+std::to_string(k)+"] starting..");
    while(1)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isEmpty())
        {
            cvar.wait(lck);
        }

        int value = q.remove();
        if(value == -1)
        {
                ConsolePrint("Consumer removed -1  (qsz : " + std::to_string(q.size()) + ")");
            break;
        }
        else
        {
            consStat.at(k) = consStat.at(k) + 1;
        }
        ConsolePrint("Consumer [" + std::to_string(k) + "] removed  " + std::to_string(value) + "  (qsz : " + std::to_string(q.size()) + ")");
        lck.unlock();
        cvar.notify_all();
    }
    ConsolePrint("Consumer ending...");
}
int main(int argc, char** argv)
{
    if (argc == 2)
    {
        numCons = atoi(argv[1]);
        if (numCons <= 0)
            return 1;
    }
    else if (argc == 1)
    {
        numCons = 1;
    }
    else
    {
        cout<<"wrong inputs! enter corrects args\n";
        return 1;
    }

    thread tProd(Producer);
    thread tCons[numCons];

    for (int i = 0; i < numCons; i++)
    {
        consStat.insert({ i, 0 });
    }

    for (int i = 0; i < numCons; i++)
    {
        tCons[i] = thread(Consumer, i);
        ConsolePrint("creating thread id for cons: " + std::to_string(i));
    }
    tProd.join();

    for (int i = 0; i < numCons; i++)
    {
        tCons[i].join();
    }

    cout<<"Consumer Stats: [ ";
    int totalSum = 0;
    for (int i = 0; i < numCons; i++)
    {
        cout<<std::to_string(consStat[i])<<", ";
        totalSum += consStat[i];
    }
    cout<<"] Total =  "<<totalSum<<"\n";
    ConsolePrint("Main: all done!");

    return 1;
}