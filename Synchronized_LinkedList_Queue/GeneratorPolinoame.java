import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class GeneratorPolinoame {
    private static String filesLocationTestCase1 = "./data/test_case1";
    private static String filesLocationTestCase2 = "./data/test_case2";

    public static void main(String[] args) {

        generateFiles(filesLocationTestCase1, 10 , 1000, 50);
        generateFiles(filesLocationTestCase2, 5 , 10000, 100);
    }
    private static void generateFiles(String filesLocation, int noPolynoms, int maxDegree, int maxMonoms){
        Random rand = new Random();

        for(int i = 0;i < noPolynoms;i++){
            List<Integer> coefficientsList = new ArrayList<>();
            List<Integer> exponentsList = new ArrayList<>();
            int currentMonomsCount = rand.nextInt(maxMonoms + 1);
            for(int j = 0; j < currentMonomsCount; j++){
                int coefficient = rand.nextInt(100 + 1);
                while(coefficient == 0)
                    coefficient = rand.nextInt(100 + 1);
                coefficient = rand.nextInt(10) % 10 < 5 ? -coefficient : coefficient;
                coefficientsList.add(coefficient);

                int exponent = rand.nextInt(maxDegree + 1);
                while(exponentsList.contains(exponent))
                    exponent = rand.nextInt(maxDegree + 1);
                exponentsList.add(exponent);
            }

            Set<Integer> exponentsSet = new HashSet<>(exponentsList);
            assert (exponentsSet.size() == exponentsList.size());
            assert (!coefficientsList.contains(0));
            assert (exponentsList.size() == coefficientsList.size());

            try {
                String path = filesLocation + "/" + "polinom" + i;
                File newFile = new File(path);
                FileWriter myWriter = new FileWriter(path);

                for(int j = 0; j < currentMonomsCount; j++){
                    myWriter.write(coefficientsList.get(j) + " " + exponentsList.get(j));
                    myWriter.write("\n");
                }

                myWriter.close();
            } catch (IOException e) {
                System.out.println("An error occurred.");
                e.printStackTrace();
            }
        }
    }

}
