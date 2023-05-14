#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <assert.h>
#include <mpi.h>

using namespace std;
#define MAX 100000
int res[MAX];
int main(int argc, char** argv)
{
    int p;
    int rank;
    int rc = MPI_Init(NULL, NULL);
    if (rc != MPI_SUCCESS) {
        cout << "Error init MPI";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int N, n1_size, n2_size;
    int current_process_rank = 1;
    auto startTime = chrono::high_resolution_clock::now();
    

    if (rank == 0) {

        string input_files_folder = "D:\\Facultate Semestrul 5\\PPD\\Lab3\\C++\\input_files\\";

        string test_case_file = argv[1];

        string first_number_txt = input_files_folder + test_case_file + "\\" + "Numar1.txt";
        string second_number_txt = input_files_folder + test_case_file + "\\" + "Numar2.txt";
        string file_output_txt = input_files_folder + test_case_file + "\\" + "Numar3.txt";

        string str_n1_size;
        string str_n2_size;
        int pos;

        ifstream File1;
        File1.open(first_number_txt, std::ifstream::binary);
        ifstream File2;
        File2.open(second_number_txt, std::ifstream::binary);
        getline(File1, str_n1_size);
        getline(File2, str_n2_size);

        n1_size = stoi(str_n1_size);
        n2_size = stoi(str_n2_size);
        N = (n1_size < n2_size) ? n2_size : n1_size;



        int chunk_size_from_n1 = n1_size / (p - 1);
        int chunk_size_from_n2 = n2_size / (p - 1);
        int rest_n1 = n1_size % (p - 1);
        int rest_n2 = n2_size % (p - 1);
        
        char* chunk_from_n1_unparsed = new char[chunk_size_from_n1 + 100]();
        char* chunk_from_n2_unparsed = new char[chunk_size_from_n2 + 100]();
        
        int times_to_read = p - 1;
       
        while (times_to_read--) {

            int curr_chunk_size_n1 = chunk_size_from_n1;
            int curr_chunk_size_n2 = chunk_size_from_n2;

            if (n1_size < n2_size && times_to_read == p - 2) {
                curr_chunk_size_n1 = n1_size;
                rest_n1 = 0;
            }

            if (n1_size < n2_size && times_to_read == p - 3) {
                memset(chunk_from_n1_unparsed, '0', n1_size * sizeof(char));
            }

            //citesc din n1
            if (rest_n1 > 0) {
                curr_chunk_size_n1 += 1;
                rest_n1--;
            }
            if (rest_n2 > 0) {
                curr_chunk_size_n2 += 1;
                rest_n2--;
            }

            if (times_to_read == p - 2) {
                File1.seekg(-curr_chunk_size_n1, std::ifstream::end);
                File2.seekg(- curr_chunk_size_n2, std::ifstream::end);
            }
            else {
                File1.seekg(-curr_chunk_size_n1, std::ifstream::cur);
                File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
            }


            if (n1_size == n2_size) {
                File1.read(chunk_from_n1_unparsed, curr_chunk_size_n1);
                File1.seekg(-curr_chunk_size_n1, std::ifstream::cur);

                File2.read(chunk_from_n2_unparsed, curr_chunk_size_n2);
                File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
            }
            else if(n1_size < n2_size) {
                if (times_to_read == p - 2) {
                    File1.read(chunk_from_n1_unparsed, curr_chunk_size_n1);
                    File2.read(chunk_from_n2_unparsed, curr_chunk_size_n2);
                    File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
                }
                else {
                    File2.read(chunk_from_n2_unparsed, curr_chunk_size_n2);
                    File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
                }
            }

            MPI_Send(&n1_size, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&n2_size, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&curr_chunk_size_n1, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&curr_chunk_size_n2, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(chunk_from_n1_unparsed, curr_chunk_size_n1, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(chunk_from_n2_unparsed, curr_chunk_size_n2, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD);

            current_process_rank++;
            
        }


        int carry = 0;
        MPI_Recv(&carry, 1, MPI_INT, p - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        ofstream outdata;
        outdata.open(file_output_txt);
        if (!outdata) { 
            cerr << "Error: file could not be opened" << endl;
        }

        if (carry > 0) {
            outdata << "1";
        }
        int bigger_chunk = (chunk_size_from_n1 < chunk_size_from_n2) ? chunk_size_from_n2 : chunk_size_from_n1;
        int* result_from_other_process = new int[bigger_chunk + 100]();
        
        
        int res_size_from_process;

        for(int i = p - 1; i >= 1; i--){
            MPI_Recv(&res_size_from_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(result_from_other_process, res_size_from_process, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0;j < res_size_from_process;j++) {
                outdata << result_from_other_process[j];
            }
        }

        File1.close();
        File2.close();
        outdata.close();
        delete[] chunk_from_n1_unparsed;
        delete[] chunk_from_n2_unparsed;
        delete[] result_from_other_process;
        
    }
    else {

        int carry = 0;
        int local_carry = 0;
        
        int curr_chunk_size_n1;
        int curr_chunk_size_n2;
        int n1_size, n2_size;
        MPI_Recv(&n1_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&n2_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        N = (n1_size < n2_size) ? n2_size : n1_size;

        MPI_Recv(&curr_chunk_size_n1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&curr_chunk_size_n2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int curr_N = (curr_chunk_size_n1 < curr_chunk_size_n2) ? curr_chunk_size_n2 : curr_chunk_size_n1;

        char* chunk_from_n1_unparsed = new char[curr_chunk_size_n1]();
        char* chunk_from_n2_unparsed = new char[curr_chunk_size_n2]();
        int* current_res = new int[curr_N]();

        MPI_Recv(chunk_from_n1_unparsed, curr_chunk_size_n1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(chunk_from_n2_unparsed, curr_chunk_size_n2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if (curr_chunk_size_n1 == curr_chunk_size_n2) {
            for (int i = curr_N - 1; i >= 0; i--) {
                int x = chunk_from_n1_unparsed[i] - '0';
                int y = chunk_from_n2_unparsed[i] - '0';
                int rez = (x + y + local_carry) % 10;
                local_carry = (x + y + local_carry) / 10;
                current_res[i] = rez;
            }
        }
        else {
            if (curr_chunk_size_n1 < curr_chunk_size_n2) {
                int curr_index_in_chunk_n1 = curr_chunk_size_n1 - 1;
                int curr_index_in_chunk_n2 = curr_chunk_size_n2 - 1;
                int index_in_res = curr_chunk_size_n2 - 1;
                while (curr_index_in_chunk_n1 >= 0) {
                    int x = chunk_from_n1_unparsed[curr_index_in_chunk_n1] - '0';
                    int y = chunk_from_n2_unparsed[curr_index_in_chunk_n2] - '0';
                    int rez = (x + y + local_carry) % 10;
                    local_carry = (x + y + local_carry) / 10;
                    current_res[index_in_res] = rez;
                    index_in_res--;
                    curr_index_in_chunk_n1--;
                    curr_index_in_chunk_n2--;
                }
                for (int i = curr_index_in_chunk_n2; i >= 0;i--) {
                    current_res[i] = (chunk_from_n2_unparsed[i] - '0' + local_carry) % 10;
                    if (local_carry == 1 && (chunk_from_n2_unparsed[i] - '0' + local_carry == 10))
                        local_carry = 1;
                    else
                        local_carry = 0;
                }
            }
        }
        
        if (rank != 1) {
            MPI_Recv(&carry, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int index = curr_N - 1;
        while (carry > 0 && index >= 0) {
            current_res[index] = (current_res[index] + carry) % 10;
            if (current_res[index] == 0) {
                carry = 1;
            }
            else {
                carry = 0;
            }
            index--;
        }
        
        if (rank != p - 1) {
            if (local_carry > 0) {
                MPI_Send(&local_carry, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            }
            else {
                MPI_Send(&carry, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            }
        }
        else {
            if (local_carry > 0) {
                MPI_Send(&local_carry, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            else {
                MPI_Send(&carry, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }


        MPI_Send(&curr_N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(current_res, curr_N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        
        delete[] chunk_from_n1_unparsed;
        delete[] chunk_from_n2_unparsed;
        delete[] current_res;
        
    }


    if (rank == 0) {
        auto endTime = chrono::high_resolution_clock::now();
        double durata = chrono::duration<double, milli>(endTime - startTime).count();
        cout << durata;
    }


    MPI_Finalize();
    return 0;
}