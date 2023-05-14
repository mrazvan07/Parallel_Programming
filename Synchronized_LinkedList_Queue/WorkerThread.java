public class WorkerThread extends Thread{
    private MyQueue queue;
    private SortedLinkedList resultList;

    public WorkerThread(MyQueue queue, SortedLinkedList resultList) {
        this.queue = queue;
        this.resultList = resultList;
    }

    @Override
    public void run() {
        super.run();
        Node node = null;
        while (!queue.getIsDone()) {
            try {
                node = queue.dequeue();
                if (node != null)
                    resultList.add(node);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        while (!queue.isEmpty()) {
            try {
                node = queue.dequeue();
                if (node != null)
                    resultList.add(node);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
