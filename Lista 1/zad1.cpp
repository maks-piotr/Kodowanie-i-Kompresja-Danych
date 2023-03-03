#include <iostream>
#include <vector>
#include <fstream>
#include<iomanip>
#include <math.h>
#include <time.h>
using namespace std;

int L[256]; //liczba wystąpień bytów
int LW[256][256]; //liczba wystąpień bytów pod warunkiem poprzednich
double P[256]; //P[i] = P(X = i)
double PW[256][256]; //PW[i][j] = P(Y = i | X = j)
double H[256]; // H(Y|x)
int main(int argc, char *argv[]) {

    ifstream file(argv[1], ios::in | ios::binary);
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
        LW[(int)u][(int)prev]++;
        //get next byte
        prev = u;
        file.read((&c), 1);
        u = c;
    }
    cout << "Liczba bytow: " << file_s << '\n';

    for (int i = 0; i <= 255; i++) {
        P[i] = (double)L[i]/(double)file_s;
        for (int j = 0; j <= 255; j++) {
            PW[i][j] = (double)LW[i][j]/(double)file_s;
        }
    }

    for (int i = 0; i <= 255; i++) {
        H[i] = 0;
        for (int j = 0; j <= 255; j++) {
            if (LW[j][i]) {
                H[i] += PW[j][i] * (-log2(PW[j][i]));
            }
        }
    }

    double entropia = 0;
    for (int i = 0; i <= 255; i++) {
        if (L[i]) {
            entropia+= P[i]*(-log2(P[i]));
        }
    }

    double entropiaWarunkowa = 0;
    for (int i = 0; i <= 255; i++) {
        entropiaWarunkowa+= P[i]*H[i];
    }


    cout << "Prawdopodobienstwo wystapienia poszczegolnych bytow: {";
    for (int i = 0; i < 255; i++) {
        cout << fixed<< setprecision(4) << P[i] << ", ";
    }
    cout << P[255] << "}\n";

    cout << "Entropia wynosi: " << setprecision(10)<< entropia << '\n';
    cout << "Entropia Warunkowa wynosi: " << entropiaWarunkowa << '\n';
    cout << "Entropia-Entropia Warunkowa wynosi: " << entropia-entropiaWarunkowa << "\n";

    return 0;
}

