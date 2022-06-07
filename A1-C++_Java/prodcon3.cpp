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
//condition_variable cvar_cons;
int BUFSIZE = 20; // queue capacity
int NUMITEMS = 100; // total number of data items
map<int, int> consStat;
Queue q(BUFSIZE);
int partitionProd;
int numCons;
int numProd;


void ConsolePrint(string message)
{
    mtxConsolePrint.lock();
    cout << message << "\n";
    mtxConsolePrint.unlock();
}
void Producer(int k)
{
    int rem = 0;

    if (k == (numProd - 1))
    {
        rem = NUMITEMS % (k + 1);
    }
    int start = k * (partitionProd);
    int end = ((k + 1) * (partitionProd)) + rem;

    ConsolePrint("Producer["+std::to_string(k)+"] segment[" + std::to_string(start + 1) + ".." + std::to_string(end) + "] starting on core "+ std::to_string(sched_getcpu()));
//    + std::to_string(sched_getcpu())
    for (int i = start; i < end; i++)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isFull())
        {
            //cvar_prod.wait(lck, []() { return !(q.isFull()); });
            cvar.wait(lck);
        }

        q.add(i);
        ConsolePrint("Producer [" + std::to_string(k) + "] added  " + std::to_string(i) + "  (qsz : " + std::to_string(q.size()) + ")");

        //cvar_cons.notify_one();
        lck.unlock();
        cvar.notify_all();

    }


    //ConsolePrint("Producer ending");
}
void Consumer(int k)
{

    while(1)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isEmpty())
        {
            //cvar_cons.wait(lck, []() { return !(q.isEmpty()); });
            cvar.wait(lck);
        }

        int value = q.remove();
        if (value == -1)
        {
            //cout<<"\n";
            ConsolePrint("Consumer  added -1   (qsz : " + std::to_string(q.size()) + ")");
            break;
        }
        else
        {
            consStat.at(k) = consStat.at(k) + 1;
        }

        ConsolePrint("Consumer [" + std::to_string(k) + "] removed  " + std::to_string(value) + "  (qsz : " + std::to_string(q.size()) + ")");

        //cvar_prod.notify_one();
        lck.unlock();
        cvar.notify_all();

    }

    ConsolePrint("Consumer ending...");
}
int main(int argc, char** argv)
{
    if (argc == 1)
    {
        numCons = 1;
        numProd = 1;
    }
    else if (argc == 2)
    {
        numCons = atoi(argv[1]);
        if (numCons < 0)
            return 1;
        numProd = 1;
    }
    else if (argc == 3)
    {
        numCons = atoi(argv[1]);
        numProd = atoi(argv[2]);
        if (numCons < 0 || numProd < 0)
            return 1;
    }
    else
    {
        cout<<"wrong inputs! enter correct inputs\n";
        return 1;
    }
    partitionProd = NUMITEMS / numProd;
    thread tCons[numCons];
    thread tProd[numProd];
    for (int i = 0; i < numProd; i++)
    {
        tProd[i] = thread(Producer, i);
        ConsolePrint("creating thread id for prod: " + std::to_string(i));
    }
    for (int i = 0; i < numCons; i++)
    {
        consStat.insert({ i, 0 });
    }

    for (int i = 0; i < numCons; i++)
    {
        tCons[i] = thread(Consumer, i);
    }

    for (int i = 0; i < numProd; i++)
    {
        tProd[i].join();
    }


    for (int i = 0; i < numCons; i++)
    {
        unique_lock<mutex> lck(mtx);
        while (q.isFull())
        {
            cvar.wait(lck);
        }
        q.add(-1);

        cvar.notify_all();
        lck.unlock();

    }
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