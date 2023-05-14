#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <assert.h>
#include <mpi.h>

using namespace std;
#define MAX 100000
int n1[MAX], n2[MAX], res[MAX];

int main(int argc, char** argv)
{
    int rank, nr_procese;
    int rc = MPI_Init(NULL, NULL);
    if (rc != MPI_SUCCESS) {
        cout << "Error init MPI";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procese);

    int N, n1_size, n2_size;
    int chunk_size;

    int current_process_rank = 1;
    int root_process_has_carry = false;
    ofstream outdata;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;


    if (rank == 0) {
        string input_files_folder = "D:\\Facultate Semestrul 5\\PPD\\Lab3\\C++\\input_files\\";

        string command_line_version = argv[1];

        string first_number_txt = input_files_folder + command_line_version + "\\" + "Numar1.txt";
        string second_number_txt = input_files_folder + command_line_version + "\\" + "Numar2.txt";
        string file_output_txt = input_files_folder + command_line_version + "\\" + "Numar3.txt";

        string str_n1_size;
        string str_n2_size;
        int pos;
        startTime = chrono::high_resolution_clock::now();
        

        ifstream File1(first_number_txt);
        ifstream File2(second_number_txt);
        outdata.open(file_output_txt);
        if (!outdata) { // file couldn't be opened
            cerr << "Error: file could not be opened" << endl;
        }
        getline(File1, str_n1_size);
        getline(File2, str_n2_size);

        n1_size = stoi(str_n1_size);
        n2_size = stoi(str_n2_size);

        N = n1_size > n2_size ? n1_size : n2_size;

        while (N % nr_procese > 0) {
            N++;
        }

        chunk_size = N / nr_procese;

        string chars1;
        string chars2;

        getline(File2, chars2);
        getline(File1, chars1);
        for (int i = 0; i < n1_size; i++) {
            n1[N - n1_size + i] = chars1.at(i) - '0';
        }

        for (int i = 0; i < n2_size; i++) {
            n2[N - n2_size + i] = chars2.at(i) - '0';
        }

        File1.close();
        File2.close();
    }

    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int carry = 0;
    int local_carry;
 
    int* current_chunk1 = (int*)malloc(sizeof(int) * chunk_size);
    int* current_chunk2 = (int*)malloc(sizeof(int) * chunk_size);
    int* current_res = (int*)malloc(sizeof(int) * chunk_size);

    MPI_Scatter(n1, chunk_size, MPI_INT, current_chunk1, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(n2, chunk_size, MPI_INT, current_chunk2, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    int i;
    for (i = chunk_size - 1; i >= 0; i--) {
        int rez = (current_chunk1[i] + current_chunk2[i] + carry) % 10;
        carry = (current_chunk1[i] + current_chunk2[i] + carry) / 10;
        current_res[i] = rez;
    }

    local_carry = carry;
    if (rank == 0) {
        if (local_carry > 0) {
            outdata << "1";
        }
    }

    if (rank != nr_procese - 1 && rank != 0) {
        MPI_Recv(&carry, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int index = chunk_size - 1;
        while (carry > 0) {
            current_res[index] = current_res[index] + carry;

            if (current_res[index] == 0) {
                carry = 1;
            }
            else {
                carry = 0;
            }
            index--;
        }

        if (local_carry > 0) {
            MPI_Send(&local_carry, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        }

        else {
            MPI_Send(&carry, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Send(&carry, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    }


    if (rank == 0) {
        MPI_Recv(&carry, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int index = chunk_size - 1;
        while (carry > 0 && index >= 0) {
            current_res[index] = current_res[index] + carry;
            if (current_res[index] == 0) {
                carry = 1;
            }
            else {
                carry = 0;
            }
            index--;
        }
        if (carry > 0 && index < 0) {
            root_process_has_carry = true;
        }
    }

    MPI_Gather(current_res, chunk_size, MPI_INT, res, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (root_process_has_carry) {
        outdata << "1";
    }
    else {
        for (int i = 0;i < N;i++) {
            outdata << res[i];
        }
    }

    if (rank == 0) {
        auto endTime = chrono::high_resolution_clock::now();
        double durata = chrono::duration<double, milli>(endTime - startTime).count();
        outdata.close();
        cout << durata;
    }
    MPI_Finalize();
    return 0;
}