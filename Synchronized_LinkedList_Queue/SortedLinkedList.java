import java.util.ArrayList;
import java.util.List;

public class SortedLinkedList {


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

    public synchronized boolean isEmpty(){
        return (head == null);
    }

    public synchronized boolean add(Node item) {
        //raise exception if null
        if ( item == null){
            throw new NullPointerException();
        }

        //raise exception if not valid object
        if (!(item instanceof Comparable)){
            throw new ClassCastException();
        }

        // make the new node to insert into list
        Node newNode = new Node(item.data);
        // first see if the list is empty
        if (isEmpty()){
            //for testing
            //System.out.println("add "+item +" to front");
            head = newNode;
            size++;
            return true;
        }
        // there is something in the list
        // now check to see if it belongs in front
        else if (item.compareTo(head) < 0)	{
            //for testing
            //System.out.println("add ("+item +") before ("+head.data+")");
            newNode.next = head;
            head = newNode;
            size++;
            return true;
        }
        // otherwise, step down the list.  n will stop
        // at the node after the new node, and trailer will
        // stop at the node before the new node
        else {
            boolean found_equal = false;
            Node after = head.next;
            Node before = head;
            while (after != null){

                if(item.compareTo(before) == 0){
                    before.addData(item.data);
                    return true;
                }

                if(item.compareTo(after) == 0) {
                    found_equal = true;
                    break;
                }
                if (item.compareTo(after) < 0)
                    break;
                before = after;
                after = after.next;
            }
            // insert between before & after

            if(found_equal == false) {
                newNode.next = before.next;
                before.next = newNode;
                size++;
            }else {
                after.addData(newNode.data);
                if (after.nullData())
                    remove(after);
            }
            //for testing
            //System.out.println("add ("+item +") after ("+before.data+")");
            return true;
        }
    }

    public synchronized void remove(int index) {
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

    public synchronized boolean remove(Node key) {
        //raise exception if null
        if ( key == null){
            throw new NullPointerException();
        }

        if(size()==0){
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
        while (temp != null && (key.compareTo(temp) != 0))
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

    public List<Node> getAllNodes(){
        Node temp = head;
        List<Node> nodes = new ArrayList<>();

        while(temp != null){
            nodes.add(temp);
            temp = temp.next;
        }

        return nodes;
    }

}