import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class ReaderThread extends Thread{

    private CyclicBarrier barrier;
    private String filesFolderPath;
    private int polynomsNo;
    private MyQueue queue;
    private int start,end;

    public ReaderThread(int start, int end, String filesFolderPath, int polynomsNo , MyQueue queue, CyclicBarrier barrier) {
        this.filesFolderPath = filesFolderPath;
        this.queue = queue;
        this.polynomsNo = polynomsNo;
        this.barrier = barrier;
        this.start = start;
        this.end = end;
    }

    @Override
    public void run() {
        try {
            super.run();
            int currentFileIndex = start;
            while (currentFileIndex < end) {
                String currentFileName = "polinom" + currentFileIndex;
                try {
                    File myObj = new File(filesFolderPath + currentFileName);
                    Scanner myReader = new Scanner(myObj);
                    while (myReader.hasNextLine()) {
                        String data = myReader.nextLine().strip();
                        if (data.isEmpty()) break;
                        String[] coefficientAndExponent = data.split(" ");
                        int coefficient = Integer.parseInt(coefficientAndExponent[0]);
                        int exponent = Integer.parseInt(coefficientAndExponent[1]);
                        Node currentNode = new Node(new Monom(coefficient, exponent));
                        queue.enqueue(currentNode);
                    }
                    myReader.close();
                } catch (FileNotFoundException e) {
                    System.out.println("An error occurred.");
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
                currentFileIndex += 1;
            }

            int await = barrier.await();
            queue.setIsDone();

        } catch (BrokenBarrierException e) {
            throw new RuntimeException(e);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public static void writeTofile(SortedLinkedList resultList, String path) {
        List<Node> nodes = resultList.getAllNodes();
        try {
            path += "result";
            File newFile = new File(path);
            FileWriter myWriter = new FileWriter(path);

            for(int i = 0;i<nodes.size();i++){
                myWriter.write(nodes.get(i).data.getCoefficient() + " " + nodes.get(i).data.getExponent());
                myWriter.write("\n");
            }

            myWriter.close();
        } catch (IOException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }
}
