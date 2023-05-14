#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;
#define MAX 100000
int result[MAX];

int main(int argc, char** argv)
{
    string input_files_folder = "D:\\Facultate Semestrul 5\\PPD\\Lab3\\C++\\input_files\\";

    string command_line_version = argv[1];
    int N, n1_size, n2_size;
    string first_number_txt = input_files_folder + command_line_version + "\\" + "Numar1.txt";
    string second_number_txt = input_files_folder + command_line_version + "\\" + "Numar2.txt";
    string file_output_txt = input_files_folder + command_line_version + "\\" + "Numar3.txt";


    auto startTime = chrono::high_resolution_clock::now();
    string str_n1_size;
    string str_n2_size;
    ifstream File1(first_number_txt);
    ifstream File2(second_number_txt);
    getline(File1, str_n1_size);
    getline(File2, str_n2_size);

    n1_size = stoi(str_n1_size);
    n2_size = stoi(str_n2_size);
    N = n1_size > n2_size ? n1_size : n2_size;

    string n1;
    string n2;

    getline(File1, n1);
    getline(File2, n2);

    ofstream outdata;
    outdata.open(file_output_txt);
    if (!outdata) {
        cerr << "Error: file could not be opened" << endl;
    }

    int carry = 0;

    // cazul in care n1 este mai mic decat n2 la lungime
    if (n1_size < n2_size) {
        int index = 1;
        while (index <= n1_size) {
            int rez = (n1[n1_size - index] - '0' + n2[n2_size - index] - '0' + carry) % 10;
            carry = (n1[n1_size - index] - '0' + n2[n2_size - index] - '0' + carry) / 10;
            result[N - index] = rez;
            index++;
        }

        int no_carry_passed_forward_in_n2 = 0;
        while (carry > 0 && index <= N) {
            int rez = (0 + n2[n2_size - index] - '0' + carry) % 10;
            carry = (0 + n2[n2_size - index] - '0' + carry) / 10;
            result[N - index] = rez;
            no_carry_passed_forward_in_n2++;
            index++;
        }

        // caz in care primele (n2_size - n1_size) cifre din n2 ar fi '9999...'
        if (carry > 0 && index > N) {
            outdata << "1";
        }

        for (int i = 0; i < N - n1_size - no_carry_passed_forward_in_n2; i++) {
            outdata << n2[i];
        }
        for (int i = N - n1_size - no_carry_passed_forward_in_n2; i < n2_size; i++) {
            outdata << result[i];
        }
    }

    // cazul in care numerele sunt egale ca lungime
    else {
        for (int i = 1; i <= N; i++) {
            int rez = (n1[n1_size - i] - '0' + n2[n2_size - i] - '0' + carry) % 10;
            carry = (n1[n1_size - i] - '0' + n2[n2_size - i] - '0' + carry) / 10;
            result[N - i] = rez;
        }
        if (carry > 0) {
            outdata << "1";
        }
        for (int i = 0;i < N;i++) {
            outdata << result[i];
        }
    }
    auto endTime = chrono::high_resolution_clock::now();
    double durata = chrono::duration<double, milli>(endTime - startTime).count();
    cout << durata;

    return 0;
}