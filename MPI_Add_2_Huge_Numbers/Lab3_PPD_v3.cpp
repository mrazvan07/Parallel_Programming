#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <assert.h>
#include <mpi.h>
#include <map>

using namespace std;
#define MAX 100000
int res[MAX];
char n1[MAX], n2[MAX];
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
    MPI_Request requests_n1[20];
    MPI_Request requests_n2[20];
    MPI_Request requests_carry[20];
    MPI_Request requests_res[20];

    MPI_Status statuses_n1[20];
    MPI_Status statuses_n2[20];
    MPI_Status statuses_carry[20];
    MPI_Status statuses_res[20];

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    if (rank == 0) {
        
        startTime = chrono::high_resolution_clock::now();
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

        std::map<int, int> sizes_res;
        map<int, int*> offsets_res;

        int times_to_read = p - 1;

        int offset_n1;
        int offset_n2;

        while (times_to_read--) {
            int curr_chunk_size_n1 = chunk_size_from_n1; 
            int curr_chunk_size_n2 = chunk_size_from_n2;
           
            if (n1_size < n2_size && times_to_read == p - 2) {
                curr_chunk_size_n1 = n1_size;
                rest_n1 = 0;
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


            if (n1_size < n2_size && times_to_read == p - 3) {
                memset(n1, '0', n1_size * sizeof(char));
            }

            offset_n1 = N - curr_chunk_size_n1;
            offset_n2 = N - curr_chunk_size_n2;

            if (times_to_read == p - 2) {
                File1.seekg(-curr_chunk_size_n1, std::ifstream::end);
                File2.seekg(-curr_chunk_size_n2, std::ifstream::end);
            }
            else {
                File1.seekg(-curr_chunk_size_n1, std::ifstream::cur);
                File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
            }


            if (n1_size == n2_size) {
                File1.read(n1 + offset_n1, curr_chunk_size_n1);
                File1.seekg(-curr_chunk_size_n1, std::ifstream::cur);

                File2.read(n2 + offset_n2, curr_chunk_size_n2);
                File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);


            }
            else if (n1_size < n2_size) {
                if (times_to_read == p - 2) {
                    File1.read(n1, curr_chunk_size_n1);
                    File2.read(n2 + offset_n2, curr_chunk_size_n2);
                    File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
                }
                else {
                    File2.read(n2 + offset_n2, curr_chunk_size_n2);
                    File2.seekg(-curr_chunk_size_n2, std::ifstream::cur);
                }
            }

            MPI_Send(&n1_size, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&n2_size, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&curr_chunk_size_n1, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);
            MPI_Send(&curr_chunk_size_n2, 1, MPI_INT, current_process_rank, 0, MPI_COMM_WORLD);

            if (n1_size < n2_size) {
                MPI_Isend(n1, curr_chunk_size_n1, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD, &requests_n1[current_process_rank]);
                MPI_Isend(n2 + offset_n2, curr_chunk_size_n2, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD, &requests_n2[current_process_rank]);
            }
            else {
                MPI_Isend(n1 + offset_n1, curr_chunk_size_n1, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD, &requests_n1[current_process_rank]);
                MPI_Isend(n2 + offset_n2, curr_chunk_size_n2, MPI_CHAR, current_process_rank, 0, MPI_COMM_WORLD, &requests_n2[current_process_rank]);
            }
            
            sizes_res[current_process_rank] = curr_chunk_size_n2;

            if (current_process_rank == 1)
                offsets_res[current_process_rank] = res + N - curr_chunk_size_n2;
            else
                offsets_res[current_process_rank] = offsets_res[current_process_rank - 1] - curr_chunk_size_n2;

            offset_n1 -= curr_chunk_size_n1;
            offset_n2 -= curr_chunk_size_n2;

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

        int offset_res = 0;
        int* buf = res;
        int last_rest_sizes[20];
        
        for (int i = p - 1; i >= 1; i--) {
            MPI_Recv(&res_size_from_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Irecv(offsets_res[i], sizes_res[i], MPI_INT, i, 0, MPI_COMM_WORLD, &requests_res[i]);
        }

        for (int i = p - 1; i >= 1; i--) {
            MPI_Wait(&requests_res[i], &statuses_res[i]);
        }

        for (int i = 0;i < N;i++) {
            outdata << res[i];
        }

        File1.close();
        File2.close();
        outdata.close();
      
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

        char* chunk_from_n1_unparsed = new char[curr_chunk_size_n1 + 100]();
        char* chunk_from_n2_unparsed = new char[curr_chunk_size_n2 + 100]();

        int* current_res = new int[curr_N]();

        MPI_Irecv(chunk_from_n1_unparsed, curr_chunk_size_n1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &requests_n1[rank]);
        MPI_Irecv(chunk_from_n2_unparsed, curr_chunk_size_n2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &requests_n1[rank]);

        if (rank != 1) {
            MPI_Irecv(&carry, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &requests_carry[rank - 1]);
        }

        MPI_Wait(&requests_n1[rank], &statuses_n1[rank]);

        int flag_carry;
        if (rank != 1) {
            MPI_Test(&requests_carry[rank - 1], &flag_carry, &statuses_carry[rank - 1]);
            if (flag_carry > 0) {
                local_carry = carry;
            }
        }

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
                MPI_Wait(&requests_carry[rank - 1], &statuses_carry[rank - 1]);
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
                if(index < 0 && carry > 1)
                    local_carry = carry;
            }

        if (rank != p - 1) {
            MPI_Isend(&local_carry, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &requests_carry[rank]);
        }
        else {
            MPI_Send(&local_carry, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        }


        MPI_Send(&curr_N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Isend(current_res, curr_N, MPI_INT, 0, 0, MPI_COMM_WORLD,&requests_res[rank]);

        delete[] chunk_from_n1_unparsed;
        delete[] chunk_from_n2_unparsed;

    }


    if (rank == 0) {
        auto endTime = chrono::high_resolution_clock::now();
        double durata = chrono::duration<double, milli>(endTime - startTime).count();
        cout << durata;
    }


    MPI_Finalize();
    return 0;
}