import java.util.Queue;
import java.util.LinkedList;
import java.lang.Thread;

public class ProdCons1 {
    static int BUFSIZE = 20; // queue capacity
    int NUMITEMS = 100; // total number of data items
    static Object lck = new Object();

    public void produce(Queue q) {
        System.out.println("Producer starting..");
        for (int i = 0; i < NUMITEMS; i++)
        {
            synchronized (lck)
            {
                while (isFull(q))
                {
                    try
                    {
                        lck.wait();
                    } catch (Exception e)
                    {
                        System.err.println(e.getMessage());
                    }
                }
            }
            synchronized (lck)
            {
                q.add(i);
                lck.notifyAll();
                System.out.println("Producer added  " + i + "  (qsz : " + q.size() + ")");
            }
        }
        System.out.println("Producer ending..");
    }
public void consume(Queue q) {
        System.out.println("Consumer starting..");
        for (int i = 0; i < NUMITEMS; i++)
        {
            synchronized (lck)
            {
                while (q.isEmpty())
                {
                    try
                    {
                        lck.wait();
                    } catch (Exception e)
                    {
                        System.err.println(e.getMessage());
                    }
                }

                int value = (Integer) q.remove();
                System.out.println("Consumer removed  " + value + "  (qsz : " + q.size() + ")");
                lck.notify();
            }

        }
        System.out.println("Consumer ending..");
    }

    public boolean isFull(Queue q) {
        if (q.size() >= BUFSIZE)
            return true;
        else
            return false;
    }
 public static void main(String[] args) throws InterruptedException {
        Queue<Integer> q = new LinkedList<>();
        ProdCons1 pc = new ProdCons1();
        Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    pc.produce(q);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }
        });
        Thread t2 = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    pc.consume(q);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }
        });

        t1.start();
        t2.start();
        t1.join();
        t2.join();

        System.out.println("Main : all done!");
    }
}