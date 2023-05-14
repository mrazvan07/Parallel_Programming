import java.util.Arrays;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MyQueue {
    /*final Lock lock = new ReentrantLock();
    final Condition notFull = lock.newCondition();
    final Condition notEmpty = lock.newCondition();*/
    private boolean isDone = false;
    static final int INITIAL_CAPACITY = 20;
    private int takePtr;
    private int putPtr;
    private int capacity;
    private int size;
    private Node elements[];

    public MyQueue() {
        this.takePtr = 0;
        this.putPtr = 0;
        this.size = 0;
        this.capacity = INITIAL_CAPACITY;
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

    public synchronized Node dequeue() throws InterruptedException {
        //lock.lock();
        try {
            while (size == 0 && !isDone) wait();

            if (isDone && size == 0)
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
