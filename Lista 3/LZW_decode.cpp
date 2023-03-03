#include <iostream>
#include <string>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>
using namespace std;


map<int, string> dictionary;
int dictionary_count = 255;
unsigned char bit_buffer = 0;
int current_bit = 0;
int fib[46];
FILE *f;
FILE *fIN;

void initDictionary() {
    for (int i = 0; i <= 255; i++) {
        string tmp(1, (unsigned char)(i));
        dictionary[i] = tmp;
    }
    int dictionary_count = 255;
}
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

int prep_fib() {
    fib[0] = 1;
    fib[1] = 1;
    for (int i =2; i< 46; i++) {
        fib[i] = fib[i-1] + fib[i-2];
        //cout << i << " " << fib[i] << endl;
    }
    return 0;
}

int decode_elias_gamma() {
    int n = 0;
    int solution = 1;
    int bit = read_bits(1);
    while (bit == 0) {
        n++;
        bit = read_bits(1);
    }
    if (bit == -1) {
        return -1;
    }
    for (int i = 0; i < n; i++) {
        solution = solution <<1;
        solution += read_bits(1);
    }
    return solution;
}

int decode_elias_delta() {
    int n = decode_elias_gamma();
    if (n == -1) {
        return -1;
    }
    int solution = 1;
    for (int i = 0; i < n-1; i++) {
        solution = solution <<1;
        int bit = read_bits(1);
        if (bit == -1) {
            return -1;
        }
        solution += bit;
    }
    return solution;
}
int decode_elias_omega() {
    int n = 1;
    int old_n =1;
    int bit = read_bits(1);
    if (bit == -1) {
        return -1;
    }
    while (bit != 0) {
        n = bit;
        for (int i =0; i < old_n; i++) {
            n = n<<1;
            bit = read_bits(1);
            if (bit == -1) {
                return -1;
            }
            n += bit;
        }
        bit = read_bits(1);
        if (bit == -1) {
            return -1;
        }
        old_n = n;
    }
    if (bit == -1) {
        return -1;
    }
    return n;
}

int decode_fib() {
    vector<bool> v;
    int bit1, bit2;
    v.push_back(read_bits(1));
    v.push_back(read_bits(1));
    while (v[v.size()-1] == 0 || v[v.size()-2] == 0) {
        int bit = read_bits(1);
        v.push_back(bool(bit));
        if (bit == -1) {
            return -1;
        }
    }
    int solution = 0;
    for (int i =0; i < v.size()-1; i++) {
        //cout << v[i];
        if (v[i]) {
            solution += fib[i];
        }
    }
    return solution;
}

int decode_number(string encoding_type) {
    int decoded = -1;
    if (encoding_type.compare("delta") == 0) {
       decoded = decode_elias_delta();
    }
    else if (encoding_type.compare("gamma") == 0) {
        decoded = decode_elias_gamma();
    }
    else if (encoding_type.compare("fib") == 0) {
        return decode_fib();
    }
    else {
        decoded = decode_elias_omega();
    }
    return decoded;
}



int decode(string input, string output, string encoding_type) {
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
    initDictionary();
    if (encoding_type == "fib") {
        prep_fib();
    }
    
    auto size = filesystem::file_size(input.c_str());
    auto size1 = size;
    cout << "Rozmiar zakodowanego pliku w bajtach: " << size << endl;

    unsigned char x;
    bool endoffile = false;
    int tmp = 0;
    string last = "";
    tmp = decode_number(encoding_type)-1;
    while (tmp != -2) {
        //cout << "tmp: " << tmp << endl;
        if (dictionary.find(tmp) == dictionary.end()) {
            //cout << "wyjatek\n";
            dictionary[++dictionary_count] = last + last[0];
            //cout << dictionary_count << " - " <<  dictionary[dictionary_count] << endl;
            //cout << tmp << " " << dictionary[tmp] << endl;
            const char* cstr = dictionary[tmp].c_str();
            fwrite(cstr, sizeof(unsigned char), dictionary[tmp].size(), f);
        }
        else {
            //cout << tmp << " " << dictionary[tmp] << endl;
            const char* cstr = dictionary[tmp].c_str();
            //cout << dictionary_count << " - " <<  dictionary[dictionary_count] << endl;
            //cout << tmp << " - " << dictionary[tmp] << endl;
            fwrite(cstr, sizeof(unsigned char), dictionary[tmp].size(), f);
            if (last != "") {
                dictionary[++dictionary_count] = last + dictionary[tmp][0];
                //cout << dictionary_count << " - " <<  dictionary[dictionary_count] << endl;
            }
        if (dictionary_count >= 10000000) {
            dictionary_count = 255;
            dictionary.clear();
            initDictionary();
        }
        }
        last = dictionary[tmp];
        tmp = decode_number(encoding_type)-1;
        

    }
    return 0;
}

int main(int argc, char** argv) {
    
    string encoding_type = "omega";
    if (argc > 3) {
        encoding_type = argv[3];
    }    
    return decode(string(argv[1]),string(argv[2]),encoding_type);
}