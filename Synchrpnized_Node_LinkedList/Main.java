import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.concurrent.CyclicBarrier;

public class Main {

    public static void main(String[] args) throws InterruptedException {

        int p = Integer.parseInt(args[0]);
        int noReaders = Integer.parseInt(args[1]);
        int testCaseNo = Integer.parseInt(args[2]);
        String path =  (testCaseNo == 1) ? "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case1\\" : "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case2\\";

        SortedLinkedList resultList = new SortedLinkedList();
        MyQueue queue = new MyQueue(testCaseNo);


        int noThreads = p;
        long startTime = System.nanoTime();
        if(p == 0)
            runSequencial(testCaseNo, resultList);
        else
            runParallel(testCaseNo,noThreads,noReaders,resultList,queue);

        long endTime = System.nanoTime();
        double time = (double) (endTime - startTime)/1E6;
        System.out.println(String.format("%.6f", time));
        ReaderThread.writeTofile(resultList,path);
        //System.out.println(resultList);

        // ==TESTS QUEUE==
        /*MyQueue myQueue = new MyQueue();
        Node<Monom> n1 = new Node<>(new Monom(13,80));
        Node<Monom> n2 = new Node<>(new Monom(13,9));
        Node<Monom> n3 = new Node<>(new Monom(13,100));
        myQueue.dequeue();
        myQueue.enqueue(n1);
        myQueue.enqueue(n2);
        myQueue.enqueue(n3);
        myQueue.dequeue();
        myQueue.dequeue();
        myQueue.enqueue(n3);
        myQueue.dequeue();
        myQueue.dequeue();
        myQueue.enqueue(n3);
        System.out.println(myQueue);*/

    }

    private static void runSequencial(int testCaseNo, SortedLinkedList resultList){
        int polynomsNo = (testCaseNo == 1) ? 10 : 5;
        String path =  (testCaseNo == 1) ? "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case1\\" : "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case2\\";

        int currentFileIndex = 0;
        while (currentFileIndex < polynomsNo){
            String currentFileName = "polinom" + currentFileIndex;
            try {
                File myObj = new File(path + currentFileName);
                Scanner myReader = new Scanner(myObj);
                while (myReader.hasNextLine()) {
                    String data = myReader.nextLine().strip();
                    if(data.isEmpty()) break;
                    String[] coefficientAndExponent = data.split(" ");
                    int coefficient = Integer.parseInt(coefficientAndExponent[0]);
                    int exponent = Integer.parseInt(coefficientAndExponent[1]);
                    Node currentNode = new Node(new Monom(coefficient,exponent));
                    resultList.add(currentNode);
                }
                myReader.close();
            } catch (FileNotFoundException e) {
                System.out.println("An error occurred.");
                e.printStackTrace();
            }

            currentFileIndex += 1;
        }

        // ====TESTS====
        /*SortedLinkedList lista = new SortedLinkedList();
        Node<Monom> n1 = new Node<>(new Monom(13,80));
        Node<Monom> n2 = new Node<>(new Monom(13,9));
        Node<Monom> n3 = new Node<>(new Monom(13,100));
        Node<Monom> n4 = new Node<>(new Monom(13,33));
        Node<Monom> n5 = new Node<>(new Monom(60,33));
        Node<Monom> n6 = new Node<>(new Monom(60,8));
        Node<Monom> n7 = new Node<>(new Monom(-60,8));
        Node<Monom> n8 = new Node<>(new Monom(-60,8));
        lista.add(n1);
        lista.add(n2);
        lista.add(n3);
        lista.add(n4);
        lista.add(n5);
        lista.add(n6);
        lista.add(n7);
        lista.add(n8);
        System.out.println(lista.toString());*/
    }

    private static void runParallel(int testCaseNo, int noThreads, int noReaders, SortedLinkedList resultList, MyQueue queue) throws InterruptedException {
        int polynomsNo = (testCaseNo == 1) ? 10 : 5;
        String path = (testCaseNo == 1) ? "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case1\\" :
                "D:\\Facultate Semestrul 5\\PPD\\Lab5\\Java\\Lab5_PPD\\data\\test_case2\\";

        CyclicBarrier barrier = new CyclicBarrier(noReaders);
        ReaderThread[] readers = new ReaderThread[noReaders];

        int chunk = polynomsNo / noReaders;
        int rest = polynomsNo % noReaders;
        int start = 0, end = 0;

        for(int i = 0;i<noReaders;i++) {
            end = start + chunk;
            if(rest > 0){
                rest --;
                end ++;
            }
            readers[i] =  new ReaderThread(start,end,path,polynomsNo,queue,barrier);
            readers[i].start();
            start = end;
        }
        WorkerThread[] workers = new WorkerThread[noThreads - noReaders];
        for(int i = 0;i<noThreads-noReaders;i++){
            workers[i] = new WorkerThread(queue,resultList);
            workers[i].start();
        }
        for(int i = 0;i<noReaders;i++) {
            readers[i].join();
        }
        for(int i = 0;i<noThreads-noReaders;i++){
            workers[i].join();
        }
    }


}
