#include <iostream>
#include <string>
#include <bitset>
#include <cmath>
#include <map>
#include <fstream>
#include <random>
#include <algorithm>
#include <math.h>
using namespace std;

unsigned char bit_buffer;
int current_bit = 0;
int double_error = 0;
FILE *f;
FILE *fIN;
int trimMSBits(unsigned char *x,int curr, int n, int maxBit) {
    int MSB = 1 << (maxBit-curr);
    int solution = 0;
    while (n > 0) {
        n--;
        solution = solution << 1;
        if (*x & MSB) {
            solution+=1;
            *x -= MSB;
        }
        MSB = MSB >> 1;
    }
    return solution;
}
long long read_bits(int n) {
    int count=0;
    long long solution=0;
    if (current_bit !=  0) {
        if (8 - current_bit <= n) {
            solution = bit_buffer;
            count += 8 - current_bit;
            current_bit = 0;
        }
        else {
            solution = trimMSBits(&bit_buffer,current_bit, n, 7);
            count += n;
            current_bit += n;
        }
    }
    while (count < n) {
        int tmp = fgetc(fIN);
        if (tmp == EOF) {
            return -1;
        }
        bit_buffer = (unsigned char)(tmp);
        //cout << "byte " << (int)bit_buffer << endl;
        current_bit = 0;
        if (n-count >= 8) {
            solution = solution << 8;
            solution |= bit_buffer;
            count += 8;
        }
        else {
            solution = solution << (n-count);
            solution |= trimMSBits(&bit_buffer,current_bit, n-count,7);
            current_bit += n-count;
            count += n-count;
        }
    }
    return solution;
}



int length(int in) {
    unsigned bits, var = (in < 0) ? -in : in;
    for(bits = 0; var != 0; ++bits) var >>= 1;
    return bits;
}
bool matrixMultiplication(int* C, int* A, int * B, int rowA, int colA, int rowB, int colB) {

    if (colA != rowB) return true;

    for (int i = 0; i < rowA; i++)
        for (int j = 0; j < colB; j++)
            for (int k = 0; k < colA; k++) {
                C[i*colB + j] += A[i*colA + k] * B[k*colB + j];
                C[i*colB + j] %= 2;
            }

    return false;
}
bool print_matrix(int* A, int rowA, int colA) {

    for (int i = 0; i < rowA; i++) {
        for (int j = 0; j < colA; j++) {
            cout << A[i*colA + j] << ", ";
        }
        cout << "\n";
    }
    cout << "\n";


    return false;
}

int decode_tab(string input, string output) {
    
    int* H = new int[8*4];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            if(j == 6-i || j == 6-(i+1) || j == 6-(i+2) || j == 6-(i+4)) {
                H[i*8 + j] = 1;
            }
            else {
                H[i*8 + j] = 0;
            }
        }
    }
    
    for (int j = 0; j < 8; j++) {
        H[3*8 + j] = 1;
    }
    //cout << "H: \n";
    //print_matrix(H,4,8);
    fIN = fopen( input.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    f = fopen( output.c_str(), "wb" );
    if (!f) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    bool exit_flag = false;
    unsigned char step = 0;
    bool step_flag = false;
    char c; int x;
    while (true) {
        int* C = new int[8];
        int* val = new int[4];
        int* ret = new int[4];
        for (int i =7; i >= 0; i--) {
            C[i] = read_bits(1);
            //cout << C[i] << ": ";
            if (C[i] == -1) {
                exit_flag = true;
            }
        }
        if (exit_flag) {
            break;
        }
        matrixMultiplication(ret,H,C,4,8,8,1);
        //print_matrix(ret,1,4);
        int poprawa = 0;
        for (int i =0; i < 3; i++) {
            poprawa += ret[i]*pow(2,2-i);
        }
        if (!ret[3] && (ret[0] || ret[1] || ret[2])) {
            double_error++;
        }
        //cout << poprawa << "\n";
        if (poprawa) {
            if (poprawa == 1) {
                C[0] = !C[0];
            }
            if (poprawa == 5) {
                C[2] = !C[2];
            }
            if (poprawa == 2) {
                C[1] = !C[1];
            }
            if (poprawa == 3) {
                C[3] = !C[3];
            }
            if (poprawa == 7) {
                C[4] = !C[4];
            }
            if (poprawa == 6) {
                C[5] = !C[5];
            }
            if (poprawa == 4) {
                C[6] = !C[6];
            }
        }
        int* C2 = new int[4];
        C2[0] = C[0];
        C2[1] = (C[1] + C[0])%2;
        C2[2] = C[5];
        C2[3] = C[6];
        /*for (int i =0; i < 4; i++) {
            cout << C2[i] << ", ";
        }*/
        if (step_flag) {
            step *= pow(2,4);
            for (int i =0; i < 4; i++) {
                step += C2[i]*pow(2,3-i);
            }
            fwrite(&step, sizeof(unsigned char), 1, f);
            step = 0;
            step_flag = false;
        }
        else if (!step) {
            for (int i =0; i < 4; i++) {
                step += C2[i]*pow(2,3-i);
            }
            step_flag = true;
        }
        //cout << "\n";
    }
    
    return 0;
}


int main(int argc, char** argv) {
    string in = argv[1];
    string out = argv[2];
    decode_tab(in,out);
    cout << "Podwójne błędy: " << double_error << "\n";
}