import com.sun.source.doctree.SummaryTree;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.Collection;
import java.util.Scanner;
import java.util.concurrent.CyclicBarrier;
import java.util.stream.Collectors;

public class Main {
    public static void main(String[] args) throws FileNotFoundException, InterruptedException {
        String filename = args[1];
        String filePath = String.format("D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\%s",filename);
        Scanner file_input = new Scanner(new File(filePath));
        //System.out.println(args.length);
        //Scanner file_input = new Scanner(new File("D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\date1.txt"));
        String rows_and_cols = file_input.nextLine().strip();
        String[] rows_and_cols_parsed = rows_and_cols.split(" ");
        int n = Integer.parseInt(rows_and_cols_parsed[0]);
        int m = Integer.parseInt(rows_and_cols_parsed[1]);

        int kernel_size = Integer.parseInt(file_input.nextLine().strip());
        int[][] kernel = readKernel(kernel_size);

        int[][] matrix = null;

        if(kernel_size == 3)
            matrix = new int[n+2][m+2];
        else if(kernel_size == 5)
            matrix = new int[n+4][m+4];

        //default, kernel_size = 3
        int row_count = 1;
        int col_count = 1;

        if(kernel_size == 5){
            row_count = 2;
            col_count = 2;
        }

        int star_row_count = row_count;
        int start_col_count = col_count;
        while(file_input.hasNextLine()){
            String new_row = file_input.nextLine().strip();
            if(new_row.isEmpty())
                break;
            String[] row_parsed = new_row.split(" ");
            for (int i = 0; i < row_parsed.length; i++){
                matrix[row_count][col_count] = Integer.parseInt(row_parsed[i]);
                col_count++;
            }
            row_count++;
            col_count = start_col_count;
        }

        borderMatrix(matrix,kernel_size);
        int[][] output_matrix = new int[n][m];

        long startTime = System.nanoTime();

        //SECVENTIAL
//        int current_row_in_output_matrix = 0;
//        int current_col_in_output_matrix = 0;
//        for (int i = star_row_count; i < n + star_row_count; i++) {
//            for (int j = start_col_count; j < m + start_col_count; j++) {
//                output_matrix[current_row_in_output_matrix][current_col_in_output_matrix] = WorkerThread.transform_pixel(i, j, kernel, matrix);
//                current_col_in_output_matrix += 1;
//            }
//            current_row_in_output_matrix += 1;
//            current_col_in_output_matrix = 0;
//        }
        int start = 0;
        int end = 0;
        if(kernel_size == 3)
            start = 1;
        else if(kernel_size == 5)
            start = 2;
        //PARALEL cu P threaduri
        int p = Integer.parseInt(args[0]);
        CyclicBarrier barrier = new CyclicBarrier(p);
        Thread[] t = new WorkerThread[p];

        if(n >= m) {
            int chunk = n / p;
            int rest = n % p;
            for (int i = 0; i < t.length; i++) {
                end = start + chunk;
                if (rest > 0) {
                    rest--;
                    end++;
                }
                t[i] = new WorkerThread(p, start, end, n, m, matrix, kernel, true, barrier);
                t[i].start();
                start = end;
            }
        }else{
            int chunk = m / p;
            int rest = m % p;
            for (int i = 0; i < t.length; i++) {
                end = start + chunk;
                if (rest > 0) {
                    rest--;
                    end++;
                }
                t[i] = new WorkerThread(p, start, end, n, m, matrix, kernel, false, barrier);
                t[i].start();
                start = end;
            }
        }

        for(int i = 0; i < t.length; i++) {
            t[i].join();
        }

        long endTime = System.nanoTime();
        double time = (double) (endTime - startTime)/1E9;
        System.out.println(String.format("%.9f", time));
        printMatrix(matrix);

    }
//    private static int transform_pixel(int i, int j, int[][] kernel, int[][] matrix) {
//        int new_pixel_val = 0;
//        int[] k_values = null;
//        int[] l_values = null;
//        if(kernel.length == 3){
//            k_values = new int[]{-1,0,1};
//            l_values = new int[]{-1,0,1};
//        }else {
//            k_values = new int[]{-2,-1,0,1,2};
//            l_values = new int[]{-2,-1,0,1,2};
//        }
//        int current_k_index = 0;
//        while(current_k_index < k_values.length){
//            for (int l_index = 0; l_index < l_values.length; l_index++)
//                new_pixel_val += kernel[current_k_index][l_index] * matrix[i+k_values[current_k_index]][j+l_values[l_index]];
//            current_k_index ++;
//        }
//        return  new_pixel_val;
//    }

    private static int[][] readKernel(int kernel_size) throws FileNotFoundException {
        Scanner file_input;
        switch (kernel_size) {
            case 3:
                file_input = new Scanner(new File("D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\kernel3.txt"));
                break;
            case 5:
                file_input = new Scanner(new File("D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\kernel5.txt"));
                break;
            default:
                return null;
        }
        int[][] kernel = new int[kernel_size][kernel_size];

        int row_count = 0;
        while (file_input.hasNextLine()) {
            String new_row = file_input.nextLine().strip();
            if (new_row.isEmpty())
                break;
            String[] row_parsed = new_row.split(" ");
            int[] row = new int[row_parsed.length];
            for (int i = 0; i < row_parsed.length; i++)
                row[i] = Integer.parseInt(row_parsed[i]);
            kernel[row_count] = row;
            row_count++;
        }
        return kernel;
    }

    private static void printMatrix(int[][] matrix) {
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[0].length; j++) {
                System.out.print(matrix[i][j] + " ");
            }
            System.out.println();
        }
    }

    private static void borderMatrix(int[][] matrix, int kernel_size){
        //K=3
        if(kernel_size == 3) {
            //BORDER ROWS without corners
            for (int j = 1; j < matrix[0].length - 1; j++) {
                matrix[0][j] = matrix[1][j]; //first row
                matrix[matrix.length - 1][j] = matrix[matrix.length - 2][j];
            }

            //BORDER COLS with corners
            for (int j = 0; j < matrix.length; j++) {
                matrix[j][0] = matrix[j][1]; //first row
                matrix[j][matrix[j].length - 1] = matrix[j][matrix.length - 2];
            }
            return;
        }
        //K=5
        //BORDER ROWS without corners
        for(int j = 2 ;j< matrix[1].length-2;j++){
            matrix[1][j] = matrix[2][j]; //first row
            matrix[matrix.length-2][j] = matrix[matrix.length-3][j]; //last row
        }

        //BORDER COLS with corners
        for(int j = 1 ;j< matrix.length-1;j++){
            matrix[j][1] = matrix[j][2]; //first col
            matrix[j][matrix[j].length-2] = matrix[j][matrix[j].length-3]; //last col
        }

        for(int j = 1 ;j < matrix[0].length-1;j++){
            matrix[0][j] = matrix[1][j]; //first row
            matrix[matrix.length-1][j] = matrix[matrix.length-2][j]; //last row
        }

        //BORDER COLS with corners
        for(int j = 0 ;j< matrix.length;j++){
            matrix[j][0] = matrix[j][1]; //first col
            matrix[j][matrix[j].length-1] = matrix[j][matrix[j].length-2]; //last col
        }
    }
}