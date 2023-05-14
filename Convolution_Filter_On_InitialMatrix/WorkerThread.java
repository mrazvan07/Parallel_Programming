import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class WorkerThread extends Thread{
    private int id, p, n,m;
    private int start, end;
    private int[][] matrix, kernel;

    private int[][] up_rows;
    private int[] current_row;
    private Map<Integer, int[]> frontiers = new HashMap<>();
    private boolean byRows;
    private CyclicBarrier barrier;

    public WorkerThread(int p, int start, int end, int n, int m, int[][] matrix, int[][] kernel, boolean byRows, CyclicBarrier barrier) {
        this.p = p;
        this.n = n;
        this.m = m;
        this.start = start;
        this.end = end;
        this.matrix = matrix;
        this.kernel = kernel;
        this.byRows = byRows;
        this.barrier = barrier;

        this.current_row = matrix[start].clone();

        if (kernel.length == 3){
            //this.frontiers.put(start-1,matrix[start-1].clone());
            this.up_rows = new int[][]{matrix[start - 1].clone()};
            this.frontiers.put(end, matrix[end].clone());
        }
        if (kernel.length == 5){
            this.up_rows = new int[][]{
                    matrix[start - 2].clone(),
                    matrix[start - 1].clone()
            };
            this.frontiers.put(end, matrix[end].clone());
            this.frontiers.put(end + 1, matrix[end + 1].clone());
        }
    }

    @Override
    public void run(){
        try {
            super.run();
//            Thread.sleep((this.getId() - 12) * 1000);
//            System.out.println(this.getId() + " aici");
            int await = barrier.await();
//            if(await == 0){
//                System.out.println("au ajuns toate " + this.getId());  // asa afiseaza "gata" doar ultimul thread ajung la bariera
//            }
//            System.out.println("gata" + this.getId()); // asa afiseaza toate "gata"

        int col_count = 1;
//        int row_count = 1;
//        int current_row_in_output_matrix = start - 1;
//        int current_col_in_output_matrix = start - 1;
        if(kernel.length == 5)
            col_count = 2;
//            row_count = 2;
//            current_row_in_output_matrix = start - 2;
//            current_col_in_output_matrix = start - 2;


        if(byRows){
            for (int i = start; i < end ; i++){
                for (int j = col_count; j < m + col_count; j++)
                    matrix[i][j] = transform_pixel(i, j, kernel, matrix);
                // uptate up_rows si current_row

                if (kernel.length == 3){
                    up_rows[0] = current_row.clone();
                }
                else {
                    up_rows[0] = up_rows[1].clone();
                    up_rows[1] = current_row.clone();
                }
                current_row = matrix[i+1].clone();
            }
            return;
        }
//
//        // pe coloane
//        current_row_in_output_matrix = 0;
//        for (int j = start; j < end; j++){
//            for (int i = row_count; i < matrix.length - row_count ; i++) {
//                output_matrix[current_row_in_output_matrix][current_col_in_output_matrix] = transform_pixel(i, j, kernel, matrix);
//                current_row_in_output_matrix += 1;
//            }
//            current_row_in_output_matrix = 0;
//            current_col_in_output_matrix += 1;
//        }
        } catch (InterruptedException | BrokenBarrierException ex){
            System.out.println(ex);
        }
    }
    public  int transform_pixel(int i, int j, int[][] kernel, int[][] matrix) {
        int new_pixel_val = 0;
        int[] k_values = null;
        int[] l_values = null;
        if(kernel.length == 3){
            k_values = new int[]{-1,0,1};
            l_values = new int[]{-1,0,1};
        }else {
            k_values = new int[]{-2,-1,0,1,2};
            l_values = new int[]{-2,-1,0,1,2};
        }

        int current_k_index = 0;
        while(current_k_index < kernel.length) { // releva mai bine faptul ca while-ul merge pe linii
            for (int l_index = 0; l_index < kernel.length; l_index++) {
                int kernel_val = kernel[current_k_index][l_index];
                if (kernel.length == 3) {
                    if (k_values[current_k_index] == -1)
                        new_pixel_val += kernel_val * up_rows[0][j + l_values[l_index]];
                    if (k_values[current_k_index] == 0)
                        new_pixel_val += kernel_val * current_row[j + l_values[l_index]];
                    if (k_values[current_k_index] == 1) {
                        if (i == end - 1) {
                            new_pixel_val += kernel_val * frontiers.get(end)[j + l_values[l_index]];
                        } else {
                            new_pixel_val += kernel_val * matrix[i + 1][j + l_values[l_index]];
                        }
                    }
                } else {
                    if (k_values[current_k_index] == -2)
                        new_pixel_val += kernel_val * up_rows[0][j + l_values[l_index]];
                    if (k_values[current_k_index] == -1)
                        new_pixel_val += kernel_val * up_rows[1][j + l_values[l_index]];
                    if (k_values[current_k_index] == 0)
                        new_pixel_val += kernel_val * current_row[j + l_values[l_index]];
                    if (k_values[current_k_index] == 1) {
                        if (i == end - 1)
                            new_pixel_val += kernel_val * frontiers.get(end)[j + l_values[l_index]];
                        else
                            new_pixel_val += kernel_val * matrix[i + 1][j + l_values[l_index]];
                    }
                    if (k_values[current_k_index] == 2) {
                        if (i == end - 2)
                            new_pixel_val += kernel_val * frontiers.get(end)[j + l_values[l_index]];
                        else if (i == end - 1)
                            new_pixel_val += kernel_val * frontiers.get(end + 1)[j + l_values[l_index]];
                        else
                            new_pixel_val += kernel_val * matrix[i + 2][j + l_values[l_index]];
                    }
                }
            }
            current_k_index++;
        }
       return new_pixel_val;
    }
}
