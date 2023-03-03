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

int encode_tab(string input, string output) {
    
    int* G = new int[8*4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 7; j++) {
            if(j == i || j == i+1 || j == i+3) {
                G[i*8 + j] = 1;
            }
            else {
                G[i*8 + j] = 0;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        G[i*8 + 7] = 1;
    }
    //cout << "G: \n";
    //print_matrix(G,4,8);
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
    char c; int x;
    while (true) {
        int* C = new int[8];
        int* val = new int[4];
        for (int i =0; i < 4; i++) {
            val[i] = read_bits(1);
            //cout << val[i] << ": ";
            if (val[i] == -1) {
                exit_flag = true;
            }
        }
        //cout << "\n";
        if (exit_flag) {
            break;
        }
        matrixMultiplication(C,val,G,1,4,4,8);
        //print_matrix(C,1,8);
        unsigned char wyn = 0;
        for (int i = 0; i < 8; i++) {
            wyn += C[i]*pow(2,i);
        }
        fwrite(&wyn, sizeof(unsigned char), 1, f);
    }
    
    return 0;
}

int readBytes(string input) {
    int c = 0;
    FILE *fIN;
    fIN = fopen( input.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    

    unsigned char z;
    bool endoffile = false;

    

    fclose(fIN);
    return 0;
}

int main(int argc, char** argv) {
    string in = argv[1];
    string out = argv[2];
    encode_tab(in,out);
}