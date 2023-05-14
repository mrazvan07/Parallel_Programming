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
            // System.out.println("aici1");
            try {
                //System.out.println("deq1  bef ");
                node = queue.dequeue();
               // System.out.println("deq1");
                if (node != null){
                    int i;
                    if (node.data.getExponent() == 52)
                        i = 0;
                    resultList.add(node);
                }
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

        // System.out.println(this.getId() + "  " + "worker done");

    }
}
