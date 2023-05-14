#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;
int n, m, kernel_size, p;
bool byRows = true;

int* rowOfStringToRowOfInts(string row, int row_of_ints_size) {
    char* ptr;
    char* arr = new char[row.length() + 1];
    strcpy(arr, row.c_str());
    ptr = strtok(arr, " ");

    int* row_of_ints = new int[row_of_ints_size];
    int i = 0;
    while (ptr != NULL)
    {
        //sscanf(ptr, "%d", &matrix_size[i]);  
        row_of_ints[i] = atoi(ptr);
        ptr = strtok(NULL, " ");
        i++;
    }
    return row_of_ints;
}

void print_kernel(int* matrix[], int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    // cout << (matrix + (rows-1)*cols + (cols-1)) - (matrix+0*cols+0);
}

void print_matrix(int* matrix[], int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    // cout << (matrix + (rows-1)*cols + (cols-1)) - (matrix+0*cols+0);
}

void borderMatrix(int* matrix[], int kernel_size, int rows, int cols) {
    //K=3
    if (kernel_size == 3) {
        //BORDER ROWS without corners
        for (int j = 1; j < cols - 1; j++) {
            //*(matrix + 0 * cols + j) = *(matrix + 1 * cols + j);
            matrix[0][j] = matrix[1][j]; //first row
            //*(matrix + (rows - 1) * cols + j) = *(matrix + (rows - 2) * cols + j);
            matrix[rows - 1][j] = matrix[rows - 2][j];
        }

        //BORDER COLS with corners
        for (int j = 0; j < rows; j++) {
            //*(matrix + j * cols + 0) = *(matrix + j * cols + 1);
            matrix[j][0] = matrix[j][1]; //first row
            //*(matrix + j * cols + cols - 1) = *(matrix + j * cols + cols - 2);
            matrix[j][cols - 1] = matrix[j][cols - 2];
        }
        return;
    }
    // K=5
    // BORDER ROWS without corners
    for (int j = 2;j < cols - 2;j++) {
        //*(matrix + 1 * cols + j) = *(matrix + 2 * cols + j);
        matrix[1][j] = matrix[2][j]; //first row
        //*(matrix + (rows - 2) * cols + j) = *(matrix + (rows - 3) * cols + j);
        matrix[rows - 2][j] = matrix[rows - 3][j]; //last row
    }

    //BORDER COLS with corners
    for (int j = 1;j < rows - 1;j++) {
        //*(matrix + j * cols + 1) = *(matrix + j * cols + 2);
        matrix[j][1] = matrix[j][2]; //first col
        //*(matrix + j * cols + (cols - 2)) = *(matrix + j * cols + (cols - 3));
        matrix[j][cols - 2] = matrix[j][cols - 3]; //last col
    }

    for (int j = 1;j < cols - 1;j++) {
        //*(matrix + 0 * cols + j) = *(matrix + 1 * cols + j);
        matrix[0][j] = matrix[1][j]; //first row
        //*(matrix + (rows - 1) * cols + j) = *(matrix + (rows - 2) * cols + j);
        matrix[rows - 1][j] = matrix[rows - 2][j]; //last row
    }

    //BORDER COLS with corners
    for (int j = 0;j < rows;j++) {
        //*(matrix + j * cols + 0) = *(matrix + j * cols + 1);
        matrix[j][0] = matrix[j][1]; //first col
        //*(matrix + j * cols + (cols - 1)) = *(matrix + j * cols + (cols - 2));
        matrix[j][cols - 1] = matrix[j][cols - 2]; //last col
    }

}


int transform_pixel(int i, int j,int* kernel[], int* matrix[]) {
    int new_pixel_val = 0;
    int* k_values;
    int* l_values;
    if (kernel_size == 3) {
        k_values = new int[3] {-1, 0, 1};
        l_values = new int[3] {-1, 0, 1};
    }
    else {
        k_values = new int[5] {-2, -1, 0, 1, 2};
        l_values = new int[5] {-2, -1, 0, 1, 2};
    }
    int current_k_index = 0;
    while (current_k_index < kernel_size) {
        for (int l_index = 0; l_index < kernel_size; l_index++)
            new_pixel_val += kernel[current_k_index][l_index] * matrix[i + k_values[current_k_index]][j + l_values[l_index]];
        current_k_index++;
    }
    return  new_pixel_val;
}

void run(int* matrix[],int* kernel[], int* output_matrix[],int start, int end) {
    int col_count = 1;
    int row_count = 1;
    int current_row_in_output_matrix = start - 1;
    int current_col_in_output_matrix = start - 1;
    if (kernel_size == 5) {
        col_count = 2;
        row_count = 2;
        current_row_in_output_matrix = start - 2;
        current_col_in_output_matrix = start - 2;
    }
    if (byRows) {
        int start_col_count = col_count;
        current_col_in_output_matrix = 0;
        for (int i = start; i < end; i++) {
            for (int j = start_col_count; j < m + start_col_count; j++) {
                output_matrix[current_row_in_output_matrix][current_col_in_output_matrix] = transform_pixel(i, j, kernel,matrix);
                current_col_in_output_matrix += 1;
            }
            current_row_in_output_matrix += 1;
            current_col_in_output_matrix = 0;
        }
        return;
    }

    // pe coloane
    current_row_in_output_matrix = 0;
    for (int j = start; j < end; j++) {
        for (int i = row_count; i < n - row_count; i++) {
            output_matrix[current_row_in_output_matrix][current_col_in_output_matrix] = transform_pixel(i, j, kernel, matrix);
            current_row_in_output_matrix += 1;
        }
        current_row_in_output_matrix = 0;
        current_col_in_output_matrix += 1;
    }
}

int main(int argc, char* argv[]) {

    p = int(*argv[1] - '0');
    string filename = argv[2];
    string filepath = "D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\";
    string file = filepath + filename;

    ifstream MyReadFile(file);
    string new_row;
    getline(MyReadFile, new_row);

    int* matrix_size = rowOfStringToRowOfInts(new_row, 2);
    n = matrix_size[0];
    m = matrix_size[1];
    delete[] matrix_size;


    getline(MyReadFile, new_row);
    int* kernel_size_unparsed = rowOfStringToRowOfInts(new_row, 1);
    kernel_size = kernel_size_unparsed[0];
    delete[] kernel_size_unparsed;
    int** kernel = new int* [kernel_size];
    for (int i = 0; i < kernel_size;i++)
        kernel[i] = new int[kernel_size];

    // readKernel
    string kernel_filepath;
    if (kernel_size == 3)
        kernel_filepath = "D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\kernel3.txt";
    else
        kernel_filepath = "D:\\Facultate Semestrul 5\\PPD\\Lab1\\Java\\lab1_PPD\\src\\resources\\kernel5.txt";

    ifstream KernelFile(kernel_filepath);
    int k_row_count = 0;
    while (getline(KernelFile, new_row)) {
        if (new_row.empty())
            break;
        int* kernel_row = rowOfStringToRowOfInts(new_row, kernel_size);
        for (int i = 0; i < kernel_size; i++)
            kernel[k_row_count][i] = kernel_row[i];
        k_row_count++;
        delete[] kernel_row;
    }


    // print_kernel(kernel, 5, 5);

     ////print_matrix((int*)&kernel,kernel_size,kernel_size);
     //const int nn = n + 2;
     //const int mm = m + 2;
     //int matrix[nn][mm];
     //// for(int i=0; i < nn; ++i){
     ////     for (int j=0; j < mm; ++j){
     ////         matrix[i][j] =  0;
     //// }
     //// }
     ////cout << sizeof(matrix)/sizeof(matrix[0]);
     ////int matrix[n+4][m+4];
     ////print_matrix((int*)&matrix,n,m);

    int** matrix;
    int current_rows;
    int current_cols;
    if (kernel_size == 3) {
        matrix = new int* [n + 2];
        for (int i = 0;i < n + 2;i++)
            matrix[i] = new int[m + 2];
        current_rows = n + 2;
        current_cols = m + 2;
    }
    if (kernel_size == 5) {
        matrix = new int* [n + 4];
        for (int i = 0;i < n + 4;i++)
            matrix[i] = new int[m + 4];
        current_rows = n + 4;
        current_cols = m + 4;
    }


    int** output_matrix = new int* [n];
    for (int i = 0; i < n; i++)
        output_matrix[i] = new int[n];


    int row_count = 1;
    int col_count = 1;
    if (kernel_size == 5) {
        row_count = 2;
        col_count = 2;
    }
    int star_row_count = row_count;
    int start_col_count = col_count;
    while (getline(MyReadFile, new_row)) {
        if (new_row.empty())
            break;
        int* matrix_row = rowOfStringToRowOfInts(new_row, m);
        for (int i = 0; i < m; i++) {
            matrix[row_count][col_count] = matrix_row[i];
            col_count++;
        }
        delete[] matrix_row;
        row_count++;
        col_count = start_col_count;
    }

    if (kernel_size == 3)
        borderMatrix(matrix, kernel_size, n + 2, m + 2);
    else
        borderMatrix(matrix, kernel_size, n + 4, m + 4);

    auto startTime = chrono::high_resolution_clock::now();


    // SECVENTIAL
    /*int current_row_in_output_matrix = 0;
    int current_col_in_output_matrix = 0;
    for (int i = star_row_count; i < n + star_row_count; i++) {
        for (int j = start_col_count; j < m + start_col_count; j++) {
            output_matrix[current_row_in_output_matrix][current_col_in_output_matrix] = transform_pixel(i, j,kernel,matrix);
            current_col_in_output_matrix += 1;
        }
        current_row_in_output_matrix += 1;
        current_col_in_output_matrix = 0;
    }*/

     //PARALEL
    int start = 0;
    int end = 0;
    if (kernel_size == 3)
        start = 1;
    else if (kernel_size == 5)
        start = 2;
    //PARALEL cu P threaduri
    vector<thread> t(p);
    if (n >= m) {
        int chunk = n / p;
        int rest = n % p;
        for (int i = 0; i < t.size(); i++) {
            end = start + chunk;
            if (rest > 0) {
                rest--;
                end++;
            }
            t[i] = thread(run, ref(matrix), ref(kernel), ref(output_matrix),start, end);
            // t[i].start();
            start = end;
        }
    }
    else {
        int chunk = m / p;
        int rest = m % p;
        for (int i = 0; i < t.size(); i++) {
            end = start + chunk;
            if (rest > 0) {
                rest--;
                end++;
            }
            byRows = false;
            t[i] = thread(run,ref(matrix), ref(kernel), ref(output_matrix), start, end);
            //t[i].start();
            start = end;
        }
    }
    for (int i = 0; i < t.size(); i++) {
        t[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();

    double durata = chrono::duration<double, milli>(endTime - startTime).count();

    cout << durata;



    //print_matrix(matrix, n, m);
    for (int i = 0; i < current_rows; i++) // To delete the inner
        // arrays
        delete[] matrix[i];
    delete[] matrix;



    for (int i = 0; i < n; i++) // To delete the inner
        // arrays
        delete[] output_matrix[i];
    delete[] output_matrix;


    for (int i = 0; i < kernel_size; i++) // To delete the inner
        // arrays
        delete[] kernel[i];
    delete[] kernel;

    KernelFile.close();
    MyReadFile.close();
    return 0;
}

