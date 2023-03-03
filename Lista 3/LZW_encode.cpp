#include <iostream>
#include <string>
#include <bitset>
#include <filesystem>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include<iomanip>
#include <math.h>
#include <time.h>
using namespace std;


int entropia(string e_file) {
    int L[256]; //liczba wystąpień bytów
    double P[256]; //P[i] = P(X = i)
    double H[256]; // H(Y|x)
    for (int i = 0; i <= 255; i++) {
        L[i] = 0;
    }
    ifstream file(e_file.c_str(), ios::in | ios::binary);
    if (!file) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    unsigned char prev = 0;
    char c;
    file.read((&c), 1);
    unsigned char u = c;
    int file_s = 0;

    while (!file.eof()){
        //process byte
        file_s++;
        L[(int)u]++;
        //get next byte
        prev = u;
        file.read((&c), 1);
        u = c;
    }
    //cout << "Liczba bytow: " << file_s << '\n';

    for (int i = 0; i <= 255; i++) {
        P[i] = (double)L[i]/(double)file_s;
    }

    double entropia = 0;
    for (int i = 0; i <= 255; i++) {
        if (L[i] && P[i]) {
            entropia+= P[i]*(-log2(P[i]));
        }
    }

    cout << "Entropia " << e_file <<" wynosi: " << setprecision(10)<< entropia << '\n';

    return 0;
}



map<string, int> dictionary;
int dictionary_count = 255;
unsigned char bit_buffer;
int current_bit = 0;
int fib[46];
FILE *f;
FILE *fIN;

void initDictionary() {
    for (int i = 0; i <= 255; i++) {
        string tmp(1, (unsigned char)(i));
        dictionary[tmp] = i;
    }
    int dictionary_count = 255;
}
void writeBit (int bit)
{
    bit_buffer = bit_buffer << 1;
    if (bit) {
        bit_buffer |= 1;
    }

  current_bit++;
  if (current_bit == 8)
  {
    fwrite (&bit_buffer, 1, 1, f);
    current_bit = 0;
    bit_buffer = 0;
  }
}
void flushBits (int x)
{
  while (current_bit)
    writeBit (x);
}

int length(int in) {
    unsigned bits, var = (in < 0) ? -in : in;
    for(bits = 0; var != 0; ++bits) var >>= 1;
    return bits;
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

int encode_elias_gamma(int in) {
    int len = length(in);
    int andVal = 1;
    for (int i =0; i < len-1; i++) {
        writeBit(0);
        andVal = andVal <<1;
    }
    
    for (int i = 0; i < len; i++) {
        //cout << in << endl;
        if (andVal & in) {
            writeBit(1);
        }
        else {
            writeBit(0);
        }
        andVal = andVal >>1;
    }
    return 0;
    
}

int encode_elias_delta(int in) {
    int len = length(in);
    encode_elias_gamma(len);
    int andVal = 1;
    for (int i =0; i < len-2; i++) {
        andVal = andVal <<1;
    }
    for (int i = 0; i < len-1; i++) {
        if (andVal & in) {
            writeBit(1);
        }
        else {
            writeBit(0);
        }
        andVal = andVal >>1;
    }
    return 0;
}

int encode_elias_omega (int in) {
    vector<bool> v;
    v.push_back(0);
    int k = in;
    while (k > 1) {
        int len = length(k);
        int c = 1;
        for (int i = 0; i < len; i++) {
            if (c & k) {
                v.push_back(1);
            }
            else {
                v.push_back(0);
            }
            c = c << 1;
        }
        k = len-1;
    }
    for (int i = v.size()-1; i >= 0; i--) {
        if (v[i]) {
            writeBit(1);
        }
        else {
            writeBit(0);
        }
    }
    return 0;
}

int encode_fib(int in) {
    vector<bool> v;
    int start = 0;
    for (int i = 45; i >= 0; i--) {
        if (fib[i] <= in) {
            start = i;
            break;
        }
    }
    v.push_back(1);
    v.push_back(1);
    in -= fib[start];
    for (int i = start-1; i >= 0; i--) {
        //cout << i << endl;
        if (fib[i] <= in && v[v.size()-1] == 0) {
            in -= fib[i];
            v.push_back(1);
        }
        else {
            v.push_back(0);
        }
    }
    for (int i= v.size()-1; i >= 0; i--) {
        if (v[i]) {
            writeBit(1);
        }
        else {
            writeBit(0);
        }
    }
    return 0;
}

int encode_number(int in, string encoding_type) {
    if (encoding_type.compare("delta") == 0) {
        return encode_elias_delta(in);
    }
    else if (encoding_type.compare("gamma") == 0) {
        return encode_elias_gamma(in);
    }
    else if (encoding_type.compare("fib") == 0) {
        return encode_fib(in);
    }
    else {
        return encode_elias_omega(in);
    }
}

void flush_encoding_number(string encoding_type) {
    if (encoding_type.compare("delta") == 0) {
        flushBits(0);
    }
    else if (encoding_type.compare("gamma") == 0) {
        flushBits(0);
    }
    else if (encoding_type.compare("fib") == 0) {
        flushBits(0);
    }
    else {
        flushBits(1);
    }
}




int encode(string input, string output, string encoding_type) {
    int c = 0;
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
    cout << "Rozmiar oryginalnego pliku w bajtach: " << size << endl;

    unsigned char x;
    bool endoffile = false;
    if (fread(&x, 1, 1, fIN) != 1) {
        cerr << "Pusty plik.\n";
        return 1;
    }
    while (!endoffile) {
        string tmp(1, x);
        while (!endoffile && dictionary.find(tmp) != dictionary.end()) {
            if (fread(&x, 1, 1, fIN) == 1) {
                tmp.push_back(x);
            }
            else {
                endoffile = true;
            }
        }
        dictionary_count++;
        dictionary[tmp] = dictionary_count;
        x = tmp[tmp.size()-1];
        tmp.pop_back();
        if (tmp != "") {
            encode_number(dictionary[tmp]+1,encoding_type);
        }
        if (dictionary_count >= 10000000) {
            //cout << "przekroczno";
            dictionary_count = 255;
            dictionary.clear();
            initDictionary();
        }
    }
    encode_number(int(x)+1,encoding_type);
    flush_encoding_number(encoding_type);
    fclose(f);
    fclose(fIN);
    auto size2 = filesystem::file_size(output.c_str());
    cout << "Zapisano do pliku " << output << '\n';
    cout << "Rozmiar zakodowanego pliku w bajtach: " << size2 << endl;
    cout << "(" << (double(size2)*100.0/size1) << "\% rozmiaru oryginału, " << "Kod " << (double(size1)/size2) << " razy mniejszy od oryginału)\n";
    entropia(input);
    entropia(output);
    return 0;
}

int main(int argc, char** argv) {
    
    string encoding_type = "omega";
    if (argc > 3) {
        encoding_type = argv[3];
    }
    return encode(string(argv[1]), string(argv[2]), encoding_type);
}