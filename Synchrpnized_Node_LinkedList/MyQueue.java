import java.util.Arrays;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MyQueue {
    final Lock lock = new ReentrantLock();
    final Condition notFull = lock.newCondition();
    final Condition notEmpty = lock.newCondition();
    private boolean isDone = false;
    static final int INITIAL_CAPACITY_TEST_CASE_1 = 20;

    static final int INITIAL_CAPACITY_TEST_CASE_2 = 30;
    private int takePtr;
    private int putPtr;
    private int capacity;
    private int size;
    private Node elements[];

    public MyQueue(int testCaseNo) {
        this.takePtr = 0;
        this.putPtr = 0;
        this.size = 0;
        this.capacity = testCaseNo == 1 ? INITIAL_CAPACITY_TEST_CASE_1 : INITIAL_CAPACITY_TEST_CASE_2;
        this.elements = new Node[capacity];
    }

    public synchronized int size(){
        return this.size;
    }

    public synchronized boolean isEmpty(){
        return size == 0;
    }

    public synchronized void enqueue(Node value) throws InterruptedException {
        //lock.lock();
        try {
            while (size == elements.length) wait();
                //notFull.await();
            //System.out.println("enq");
            if (size == elements.length)
                return;

            elements[putPtr] = value;
            if(++putPtr == elements.length) putPtr = 0;
            ++ size;

            //notEmpty.signal();
            notifyAll();
        }finally {
            //lock.unlock();
            //notifyAll();
        }
    }

    /*public synchronized void resize(int new_size) {
        Node [] new_array = new Node[new_size];
        for (int i = start; i <= end; i++) {
            new_array[i - start] = this.elements[i];
        }

        *//*if (end < start) {
            for (int j = 0; j < end; j++) {
                new_array[i] = q[j];
                i++;
            }
        }*//*

        start = 0;
        this.elements = new_array;
        this.capacity = new_size;
    }*/

    public synchronized Node dequeue() throws InterruptedException {
        //lock.lock();
        try {
            while (size == 0 && !isDone) wait();
                //notEmpty.await();
            // System.out.println("deq");

            if (isDone && size == 0)
                return null;

            if(size == 0)
                return null;

            Node x = elements[takePtr];
            if (++takePtr == elements.length) takePtr = 0;
            -- size;

            //notFull.signal();
            notifyAll();
            return x;
        }finally {
            //lock.unlock();
        }
    }

  /*  public synchronized Node peek(){
        return this.elements[start];
    }*/

    public synchronized void setIsDone(){
        this.isDone = true;
        //notFull.signalAll();
        //notEmpty.signalAll();
        notifyAll();
    }

    public synchronized boolean getIsDone(){
        return this.isDone;
    }

    @Override
    public String toString() {
        return "MyQueue{" +
                "elements=" + Arrays.toString(elements) +
                '}';
    }
}
