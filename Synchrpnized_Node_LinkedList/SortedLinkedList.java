import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SortedLinkedList {

    Lock size_lock = new ReentrantLock();
    //size of the linked list
    private int size;

    // a reference to the first node in the list
    private Node head;

    //default constructor
    public SortedLinkedList()
    {
        head=null;
        size = 0;
    }

    /* isEmpty
     * inputs: none
     * return value: returns true if there are no items in linked list
     */
    public boolean isEmpty(){
        return (head == null);
    }

    public boolean add(Node newNode) {
        if (head == null){
            head = newNode;
            size++;
            return true;
        }
        else {

            if (head.data.getExponent() < newNode.data.getExponent()) {
                newNode.next = head;
                head = newNode;
                size++;
                return true;
            }
            if (head.data.getExponent() == newNode.data.getExponent()) {
                if (size == 1) {
                    if (head.data.getCoefficient() + newNode.data.getCoefficient() == 0) {
                        head = null;
                        size = 0;
                        return true;
                    }
                    head.addData(newNode.data);
                }
            }

            Node prev = head;
            Node after = head.next;

            prev.lock.lock();
            while (after != null && newNode.data.getExponent() <= after.data.getExponent()) {
                Node temp = prev;
                prev = after;
                after = after.next;
                prev.lock.lock();
                temp.lock.unlock();

                if (after != null && newNode.data.getExponent() == after.data.getExponent()) {
                    break;
                }
            }

            if (after == null) {
                prev.next = newNode;
                size++;
                prev.lock.unlock();
                return true;
            }

            if (newNode.data.getExponent() == after.data.getExponent()) {
                if (newNode.data.getCoefficient() + after.data.getCoefficient() == 0) {
                    if (after.next == null) {
                        prev.next = null;
                        size--;
                        prev.lock.unlock();
                        return true;
                    }
                    prev.next = after.next;
                    size--;
                    prev.lock.unlock();
                    return true;
                }
                after.addData(newNode.data);
                prev.lock.unlock();
                return true;
            }
            newNode.next = after;
            prev.next = newNode;
            size++;
            prev.lock.unlock();
            return true;
        }
    }

    public void remove(int index) {
        if(index < 0 || index >= size()){
            throw new IndexOutOfBoundsException();
        }

        Node ptr=head, prev = null;
        //if remove first node
        if (index == 0) {
            head = head.next;
            size--;
        }
        //less remove any other node
        for (int i = 0; i<index; i++){
            prev = ptr;
            ptr = ptr.next;
        }

        // Unlink the node from linked list
        prev.next = ptr.next;
        size-- ;
    }

    public List<Node> getAllNodes(){
        Node temp = head;
        List<Node> nodes = new ArrayList<>();

        while(temp != null){
            nodes.add(temp);
            temp = temp.next;
        }

        return nodes;
    }

    /*
    @Override
    public void clear() {
        head=null;
        size = 0;
    }

    @Override
    public boolean contains(Object o) {
        //raise exception if null
        if ( o == null){
            throw new NullPointerException();
        }

        if(size()==0){
            return false;
        }
        Node ptr = head;

        if(head.data.getClass()!=o.getClass()){
            throw new ClassCastException();
        }

        // for each node in the linked list
        while (ptr != null)
        {
            // if it is equal, return true
            if (o.equals(ptr.data))
                return true;
            ptr = ptr.next;
        }
        return false;
    }

    @Override
    public Comparable get(int index) {
        if(index < 0 || index >= size()){
            throw new IndexOutOfBoundsException();
        }

        Node ptr = head;
        //iterate to the index and return data
        for(int i=0;i<index;i++){
            ptr = ptr.next;
        }
        return (Comparable) ptr.data;
    }

    @Override
    public int indexOf(Object o) {
        //if null
        if ( o == null){
            return -1;
        }

        if(size()==0){
            return -1;
        }

        Node ptr = head;

        //if not valid object
        if(head.data.getClass()!=o.getClass()){
            return -1;
        }

        // for each node in the linked list
        int index=0;
        while (ptr != null)
        {
            // if it is equal, return true
            if (o.equals(ptr.data))
                return index;
            index++;
            ptr = ptr.next;
        }
        return -1;
    }


    @Override
    public boolean remove(Object key) {
        //raise exception if null
        if ( key == null){
            throw new NullPointerException();
        }

        if(size()==0){
            return false;
        }

        // Store head node
        Node temp = head, prev = null;

        //raise exception if not valid object
        if(head.data.getClass()!=key.getClass()){
            throw new ClassCastException();
        }

        // If head node itself holds the key to be deleted
        if (temp != null && key.equals(temp.data))
        {
            head = temp.next; // Changed head
            return true;
        }

        // Search for the key to be deleted, keep track of the
        // previous node as we need to change temp.next
        while (temp != null && (!key.equals(temp.data)))
        {
            prev = temp;
            temp = temp.next;
        }

        // If key was not present in linked list
        if (temp == null) return false;

        // Unlink the node from linked list
        prev.next = temp.next;
        return true;
    }

    @Override
    public int size() {
        return size;
    }
*/

    public boolean remove(Node key) {
        //raise exception if null
        if ( key == null){
            throw new NullPointerException();
        }

        if(size==0){
            System.out.println("kkk");
            return false;
        }

        // Store head node
        Node temp = head, prev = null;


        // If head node itself holds the key to be deleted
        if (temp != null && key.compareTo(temp) == 0)
        {
            head = temp.next; // Changed head
            return true;
        }

        // Search for the key to be deleted, keep track of the
        // previous node as we need to change temp.next
        while (temp != null && key.data.getExponent() != temp.data.getExponent())
        {
            prev = temp;
            temp = temp.next;
        }

        // If key was not present in linked list
        if (temp == null) {
            System.out.println("astaaa");
            return false;}

        // Unlink the node from linked list
        prev.lock.lock();
        prev.next = temp.next;
        prev.lock.unlock();
        return true;
    }

    public synchronized int size() {
        return size;
    }

    @Override
    public String toString(){
        String s="[";
        Node temp = head;
        if (temp!=null){
            s = String.format("%s%s", s, temp.data);
            temp=temp.next;
        }
        while(temp!=null){
            s = String.format("%s, %s", s, temp.data);
            temp=temp.next;
        }

        s = String.format("%s]",s);
        return s;
    }

}