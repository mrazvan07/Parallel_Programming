public class Monom implements Comparable<Monom> {

    private int coefficient;
    private int exponent;

    public Monom(int coefficient, int exponent) {
        this.coefficient = coefficient;
        this.exponent = exponent;
    }

    public int getCoefficient() {
        return coefficient;
    }

    public void setCoefficient(int coefficient) {
        this.coefficient = coefficient;
    }

    public int getExponent() {
        return exponent;
    }

    public void setExponent(int exponent) {
        this.exponent = exponent;
    }

    @Override
    public int compareTo(Monom o) {
        if (exponent < o.exponent)
            return 1;
        else if (exponent > o.exponent)
            return -1;
        return 0;
    }

    @Override
    public String toString() {
        return "Monom{" +
                "coefficient=" + coefficient +
                ", exponent=" + exponent +
                '}' + "\n";
    }

    public void addMonoms(Monom o){
        if (o.exponent != this.exponent)
            return;
        this.coefficient += o.coefficient;
    }

    public boolean nullMonom() {
        if (coefficient == 0)
            return true;
        return false;
    }
}
