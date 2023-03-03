#include <iostream>
#include <string>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <map>
#include <algorithm>
#include <vector>
using namespace std;

unsigned char header[1024];
int dictionary_count = 255;
unsigned char bit_buffer = 0;
int current_bit = 0;
int*** bgr;
int*** pasmo_srednie;
int*** pasmo_roznica;
int x;
int y;
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


int init_tabs() {
    pasmo_roznica = new int**[3];
    pasmo_srednie = new int**[3];
    for (int j = 0; j < 3; j++) {
        pasmo_roznica[j] = new int*[x];
        pasmo_srednie[j] = new int*[x];
        for(int i = 0; i < x; i++) {
            pasmo_roznica[j][i] = new int[y];
            pasmo_srednie[j][i] = new int[y];
        }
    }
    bgr = new int**[x];
    for (int j = 0; j < x; j++) {
        bgr[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            bgr[j][i] = new int[3];
        }
    }
    return 0;
}

int write_tab_to_file(int*** arr, string file_name) {
    cout << "writing\n";
    FILE * fOUT = fopen(file_name.c_str(),"w");
    fwrite(header,1,18,fOUT);
    for (int i = 0; i <x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < 3; k++) {
                fwrite(&arr[i][j][k],1,1,fOUT);
            }
        }
    }
    return 0;
}
int decode(string input, string output) {
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
    
    int a=0; int b=0;
    int number_of_bits = 0;
    int t=1;
    while (a == b) {
        a =read_bits(1);
        b = read_bits(1);
        if (a == b) {
            number_of_bits += a*t;
        }
        t = t << 1;
    }
    if (current_bit) {
        read_bits(8-current_bit);
    }
    cout << number_of_bits << "\n";
    int tab_size = pow(2,number_of_bits);
    vector<int> values_sre;
    vector<int> values_roz;
    bool positive_flag = false;
    for (int i = 0; i< tab_size; i++) {
        int x = int(fgetc(fIN));
        if (i != 0 && x > abs(values_sre[values_sre.size()-1])) {
            positive_flag = true;
        }
        if (!positive_flag) {
            x = -x;
        }
        values_sre.push_back(x);
        cout << x << ", ";
    }
    sort( values_sre.begin(), values_sre.end() );
    positive_flag = false;
    for (int i = 0; i< tab_size; i++) {
        int x = int(fgetc(fIN));
        if (i != 0 && x > abs(values_roz[values_roz.size()-1])) {
            positive_flag = true;
        }
        if (!positive_flag) {
            x = -x;
        }
        values_roz.push_back(x);
        cout << x << ", ";
    }
    sort( values_roz.begin(), values_roz.end() );
    cout << "\n";
    fread(header, 1, 18, fIN);
    y = int(header[12]);
    y += int(header[13]) * 256;
    std::cout <<"y: " << y << "\n";
    x = int(header[14]);
    x += int(header[15]) * 256;
    std::cout <<"x: " << x << "\n";
    init_tabs();
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i <x; i++) {
            for (int j = 0; j < y; j+=2) {
                    //cout << i << " " << j << "\n" << flush;
                    int t = 1;
                    int idx = 0;
                    for (int l =0; l < number_of_bits; l++) {
                        idx += read_bits(1) * t;
                        t *= 2;
                    }
                    //cout << "test2\n" << flush;
                    pasmo_roznica[k][i][j] = values_roz[idx];
                    //cout << k << " " << i << " " << j << " roznica " << pasmo_roznica[k][i][j] << "\n";
                }
        }
    }
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i <x; i++) {
            for (int j = 0; j < y; j+=2) {
                    int t = 1;
                    int idx = 0;
                    for (int l =0; l < number_of_bits; l++) {
                        idx += read_bits(1) * t;
                        t *= 2;
                    }
                    pasmo_srednie[k][i][j] = values_sre[idx];
                    //cout << k << " " << i << " " << j << "idx " << idx <<" srednia " << pasmo_srednie[k][i][j] << "\n";
                }
        }
    }
    if (current_bit) {
        read_bits(8-current_bit);
    }
    std::fread(&pasmo_srednie[0][0][0],1,1,f);
    std::fread(&pasmo_srednie[1][0][0],1,1,f);
    std::fread(&pasmo_srednie[2][0][0],1,1,f);

    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                cout << "predecoded srednie " << pasmo_srednie[k][i][j] << "\n";
                if (j > 1) {
                    pasmo_srednie[k][i][j] = pasmo_srednie[k][i][j] + pasmo_srednie[k][i][j-2];
                }
                else if (i > 0){
                    pasmo_srednie[k][i][j] = pasmo_srednie[k][i][j] + pasmo_srednie[k][i-1][j];
                }
                cout << "decoded srednie " << pasmo_srednie[k][i][j] << "\n";
                
                //pasmo_srednie[k][i][j] = min(pasmo_srednie[k][i][j],255);
                //pasmo_srednie[k][i][j] = max(pasmo_srednie[k][i][j],0);
            }
        }
    }
    cout << "test1\n" << flush;
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < 3; k++) {
                bgr[i][j][k] = 0;
                //std::cout << i << " " << j << "\n" << flush;
                if (j%2 == 0) {
                    bgr[i][j][k] = pasmo_srednie[k][i][j] + pasmo_roznica[k][i][j];
                }
                else if (j+1 < y) {
                    bgr[i][j][k] = pasmo_srednie[k][i][j+1] - pasmo_roznica[k][i][j+1];
                }
                else if (!(j==y-1 && (y-1)%2!=0)) {
                    cout << "testp\n" << flush;
                    bgr[i][j][k] = pasmo_srednie[i][j][k];
                }
                bgr[i][j][k] = min(bgr[i][j][k] ,255);
                bgr[i][j][k]  = max(bgr[i][j][k] ,0);
            }
        }
    }
    
    
    
    
    cout << "test2\n" << flush;
    write_tab_to_file(bgr,output);
    
    
    return 0;
}

int main(int argc, char** argv) {
    

    return decode(string(argv[1]),string(argv[2]));
}