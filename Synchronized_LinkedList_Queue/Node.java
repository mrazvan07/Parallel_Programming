public class Node<E extends Comparable<E>> implements Comparable<Node>{
    Monom data;
    Node<Monom> next;

    Node (Monom newData){
        this.data=newData;
        this.next=null;
    }

    @Override
    public int compareTo(Node o) {
        return this.data.compareTo(o.data);
    }

    @Override
    public String toString() {
        return this.data.toString();
    }

    public void addData(Monom newData){
        this.data.addMonoms(newData);
    }

    public boolean nullData(){
        return data.nullMonom();
    }
}
