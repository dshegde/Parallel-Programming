import java.util.*;
import java.lang.Thread;

public class ProdCons2 {
    static int BUFSIZE = 20; // queue capacity
    static int NUMITEMS = 100; // total number of data items
    static Object lck = new Object();
    static Object lck1 = new Object();
    static HashMap<Integer,Integer> consStat = new HashMap<>();
    static Queue<Integer> q;
    static int numCons;
    int value;

    public void ConsolePrint(String message)
    {
        synchronized (lck1)
        {
            System.out.println(message);
        }
    }
 public void produce() throws InterruptedException{
        ConsolePrint("Producer starting..");
        for (int i = 0; i < NUMITEMS; i++) {
            synchronized (lck)
            {
                while (isFull(q))
                {
                    lck.wait();
                }
            }

            synchronized (lck)
            {
                q.add(i);
                lck.notifyAll();
                ConsolePrint("Producer added  " + i + "  (qsz : " + q.size() + ")");
            }
        }

        for (int i = 0; i < numCons; i++)
        {
            synchronized (lck)
            {
                while (isFull(q))
                {
                    lck.wait();
                }
                lck.notifyAll();
                q.add(-1);
                lck.notifyAll();
            }
        }
        ConsolePrint("Producer ending..");
    }
 public void consume(int k) throws InterruptedException{
        ConsolePrint("Consumer starting..");
        while (true) {
            synchronized (lck)
            {
                while (q.isEmpty())
                {
                    lck.wait();
                }

                value = (Integer) q.remove();
                if (value == -1)
                {
                        ConsolePrint("Consumer [" + k + "] removed  " + value + " and ending..  (qsz : " + q.size() + ")");
                    break;
                }
                int temp = consStat.get(k);
                consStat.put(k, temp + 1);
                lck.notifyAll();
                ConsolePrint("Consumer [" + k + "] removed  " + value + "  (qsz : " + q.size() + ")");
            }
        }
    }

    public boolean isFull(Queue q) {
        if (q.size() >= BUFSIZE)
            return true;
        else
            return false;
    }
public static void main(String[] args) throws InterruptedException {
        q = new LinkedList<>();
        ProdCons2 pc = new ProdCons2();

        if (args.length == 1)
        {
            numCons = Integer.parseInt(args[0]);
            if (numCons < 0)
                return;
        }
        else if (args.length == 0)
        {
            numCons = 1;
        }
        else
        {
            System.out.println("wrong input! enter correct args");
            return;
        }

        for (int i=0; i<numCons; i++)
        {
            consStat.put(i,0);
        }

        Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    pc.produce();
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }
        });
        t1.start();
        Thread tCons[] = new Thread[numCons];
        for (int i=0; i<numCons; i++)
        {
            int temp = i;
            tCons[i] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        pc.consume(temp);
                    } catch (Exception e) {
                        System.err.println(e.getMessage());
                    }
                }
            });
            tCons[i].start();
        }

        t1.join();
        for (int i=0; i<numCons; i++)
        {
            tCons[i].join();
        }

        int TotalSum = 0;
        System.out.print("Consumer Stats: [ ");
        for (int i=0; i<numCons; i++)
        {
            System.out.print(consStat.get(i)+ ", ");
            TotalSum += consStat.get(i);
        }
        System.out.println("] Total = "+TotalSum);
        System.out.println("Main : all done!");
    }
}