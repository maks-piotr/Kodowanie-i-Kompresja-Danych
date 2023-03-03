#include <iostream>
#include <string>
#include <bitset>
#include <cmath>
#include <map>
#include <filesystem>
#include <random>
#include <algorithm>
#include <math.h>
using namespace std;

int*** bgr1;
int*** bgr2;
unsigned char header[1024];
int x,y;

int d2(int x1, int y1, int z1, int x2, int y2, int z2) {
    int xdiff = x2-x1;
    int ydiff = y2-y1;
    int zdiff = z2-z1;
    return (xdiff)*(xdiff) + (ydiff)*(ydiff) + (zdiff)*(zdiff);
}

void init_tab(int*** tab) {
    tab = new int**[x];
    for (int j = 0; j < x; j++) {
        tab[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            tab[j][i] = new int[3];
        }
    }
}

int readBytes(string input, int n) {
    int c = 0;
    FILE *fIN;
    fIN = fopen( input.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    
    
    unsigned char z;
    bool endoffile = false;

    fread(header, 1, 18, fIN);
    y = int(header[12]);
    y += int(header[13]) * 256;
    //std::cout <<"y: " << y << "\n";
    x = int(header[14]);
    x += int(header[15]) * 256;
    //std::cout <<"x: " << x << "\n";
    if (bgr1 == nullptr) {
    bgr1 = new int**[x];
    for (int j = 0; j < x; j++) {
        bgr1[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            bgr1[j][i] = new int[3];
        }
    }
    bgr2 = new int**[x];
    for (int j = 0; j < x; j++) {
        bgr2[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            bgr2[j][i] = new int[3];
        }
    }
    }
    if (n == 1) {
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                for (int k = 0; k < 3; k++) {
                    //cout << i << ", " << j << "\n" << flush;
                    fread(&z, 1, 1, fIN);
                    bgr1[i][j][k] = z;
                }
            }
        }
    }
    else {
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                for (int k = 0; k < 3; k++) {
                    //cout << i << ", " << j << "\n" << flush;
                    fread(&z, 1, 1, fIN);
                    bgr2[i][j][k] = z;
                }
            }
        }
    }

    fclose(fIN);
    return 0;
}

int display_info(string original, string decoded) {
    
    readBytes(original,1);
    readBytes(decoded,2);
    double mse = 0;
    double signal = 0;
    //cout << "test\n" << flush;
    //cout << "bgr" << bgr1[0][0][0] << "\n";
    if (y%2 == 0) {
        y-=1;
    }
    for (int i = 0; i <x; i++) {
        for (int j = 0; j < y; j++) {
            //cout << i << ", " << j << "\n" << flush;
            mse += d2(bgr1[i][j][0], bgr1[i][j][1], bgr1[i][j][2], bgr2[i][j][0], bgr2[i][j][1], bgr2[i][j][2]);
            signal += bgr1[i][j][0]*bgr1[i][j][0] + bgr1[i][j][1]*bgr1[i][j][1] + bgr1[i][j][2]*bgr1[i][j][2];
        }
    }
    mse /= x*y;
    signal /= x*y;
    cout << "Błąd średniokwadratowy " << decoded << ": " << mse << "\n";
    double snr = signal/mse;
    cout << "SNR: " << snr << "\n";
    cout << "SNR(dB): " <<  10.0*log10(snr) << "\n";
    return 0;
}

int main(int argc, char** argv) {
    string original = argv[1];
    string decoded = argv[2];
    display_info(original, decoded);
    //write_tab_to_file(bgr2,output);
    

    
}