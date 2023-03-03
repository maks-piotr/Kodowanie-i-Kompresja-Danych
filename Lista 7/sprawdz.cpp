#include <iostream>
#include <string>
#include <bitset>
#include <cmath>
#include <map>
#include <fstream>
#include <random>
#include <algorithm>
#include <math.h>
#include <filesystem>
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


using namespace std;
int main(int argc, char** argv) {
    string in = argv[1];
    string out = argv[2];
    fIN = fopen( in.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    f = fopen( out.c_str(), "rb" );
    if (!f) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    bool stop_flag = false;
    int counter = 0;
    while (true) {
        unsigned char c1;
        unsigned char c2;
        int r1 = fread(&c1, sizeof(unsigned char), 1,f);
        int r2 = fread(&c2, sizeof(unsigned char), 1,fIN);
        if (r1 != 1 || r2 != 1) {
            break;
        }
        //cout << int(c1) << " " << int(c2) << "\n";
        if (c1%16 != c2%16) {
            counter++;
        }
        c1 /= 16;
        c2 /= 16;
        if (c1 != c2) {
            counter++;
        }
        
    }
    auto size = filesystem::file_size(in.c_str());
    cout << "Licba róźniących się 4-elementowych bloków: " << counter <<"\n";
    cout << 100*float(counter)/(size*2) << "% wszystkich bloków" << "\n\n";
}