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


unsigned char header[1024];
int*** bgr;
int*** bgr2;
int x;
int y;
int getval(int*** arr, int a, int b, int c) {
    if (0 <=a && a < x && 0 <= b && b < y) {
        return arr[a][b][c];
    }
    return 0;
}
int d2(int x1, int y1, int z1, int x2, int y2, int z2) {
    int xdiff = x2-x1;
    int ydiff = y2-y1;
    int zdiff = z2-z1;
    return (xdiff)*(xdiff) + (ydiff)*(ydiff) + (zdiff)*(zdiff);
}
int d_tax(int x1, int y1, int z1, int x2, int y2, int z2) {
    return abs(x2 - x1) + abs(y2 -y1) + abs(z2 -z1);
}
int LGB(double threshold, int number_of_bits, int training_size, int** training, int** codebook, int seed) {
    std::random_device device;
    int mt_init = seed;
    if (seed == 0) {
        mt_init = device();
    }
    std::mt19937 generator(mt_init);
    int codebook_size = pow(2,number_of_bits);
    vector<int>* V = new vector<int>[codebook_size];
    vector<int> D; D.push_back(0);
    

    int k =0;
    while (true) {
        for (int i =0; i < training_size; i++) {
            int minimum = d_tax(training[i][0], training[i][1], training[i][2], codebook[0][0], codebook[0][1], codebook[0][2]);
            int region = 0;
            for (int j =1; j < codebook_size; j++) {
                int dij = d_tax(training[i][0], training[i][1], training[i][2], codebook[j][0], codebook[j][1], codebook[j][2]);
                if (dij < minimum) {
                    region = j;
                    minimum = dij;
                    if (minimum == 0) {
                        break;
                    }
                }
            }
            V[region].push_back(i);
        }
        int Dk = 0;
        for (int i = 0; i < codebook_size; i++) {
            for (int j =0; j< training_size; j++) {
                if (find(V[i].begin(), V[i].end(), j) != V[i].end()) {
                    Dk += d2(codebook[i][0], codebook[i][1], codebook[i][2], training[j][0], training[j][1], training[j][2]);
                }
            }
        }
        Dk /= codebook_size;
        D.push_back(Dk);
        if (D[D.size()-1] == 0 || abs(double(D[D.size()-1] - D[D.size()-2])/D[D.size()-1]) < threshold) {
            break;
        }
        k++;
        for (int i =0 ; i< codebook_size; i++) {
            if (V[i].size() == 0) {
                int maxsize = V[0].size();
                int maxj = 0;
                for (int j =1 ; j < codebook_size; j++) {
                    if (V[j].size() > maxsize) {
                        maxsize = V[j].size();
                        maxj = j; 
                    }
                }
                std::uniform_int_distribution<> dis(0, maxsize-1);
                int r = dis(generator);
                codebook[i][0] = training[V[maxj][r]][0];
                codebook[i][1] = training[V[maxj][r]][1];
                codebook[i][2] = training[V[maxj][r]][2];
            }
            else {
                int sumx = 0, sumy = 0, sumz = 0;
                for (int j =0; j < V[i].size(); j++) {
                    sumx += training[V[i][j]][0];
                    sumy += training[V[i][j]][1];
                    sumz += training[V[i][j]][2];
                }
                codebook[i][0] = sumx/V[i].size();
                codebook[i][1] = sumy/V[i].size();
                codebook[i][2] = sumz/V[i].size();
            }
            
        }
        for (int i =0 ; i< codebook_size; i++) {
            V[i].clear();
        }
    }
    delete [] V;
    return 0;
}
int encode_tab(int*** arr1, int*** arr2, int number_of_bits, int training_size, int seed) {
    std::random_device device;
    int mt_init = seed;
    if (seed == 0) {
        mt_init = device();
    }
    std::mt19937 generator(mt_init);
    std::uniform_int_distribution<int> distribution_color(0,255);
    std::uniform_int_distribution<int> distribution_x(0,x-1);
    std::uniform_int_distribution<int> distribution_y(0,y-1);
    int codebook_size = pow(2,number_of_bits);
    training_size = min(x*y,training_size);
    int** training = new int*[training_size];
    vector<pair<int,int> > pixels;
    for (int i = 0; i < x; i++) {
        for (int j =0; j < y; j++) {
            pixels.push_back(make_pair(i,j));
        }
    }
    for (int i =0; i < training_size; i++) {
        std::uniform_int_distribution<int> distribution_pixels(0,pixels.size()-1);
        training[i] = new int[3];
        int p = distribution_pixels(generator);
        training[i][0] = arr1[pixels[p].first][pixels[p].second][0];
        training[i][1] = arr1[pixels[p].first][pixels[p].second][1];
        training[i][2] = arr1[pixels[p].first][pixels[p].second][2];
        pixels.erase(pixels.begin() + p);
    }
    int** codebook = new int*[codebook_size];
    for (int i =0; i < codebook_size; i++) {
        codebook[i] = new int[3];
    }
    double threshold = 0.001;
    bool* used_colors = new bool[256*256*256]();
    int perturbation = 10;
    codebook_size = 1;
    for (int i = 0; i < training_size; i++) {
        codebook[0][0] += training[i][0];
        codebook[0][1] += training[i][1];
        codebook[0][2] += training[i][2];
    }
    codebook[0][0] /= training_size;
    codebook[0][1] /= training_size;
    codebook[0][2] /= training_size;
    for (int i =1; i <= number_of_bits; i++) {
        
        codebook_size = pow(2,i);
        for (int j = codebook_size/2; j < codebook_size; j++) {
            codebook[j][0] = codebook[j-(codebook_size/2)][0] + perturbation;
            codebook[j][0] %= 256;
            codebook[j][1] += codebook[j-(codebook_size/2)][0] + perturbation;
            codebook[j][1] %= 256;
            codebook[j][2] += codebook[j-(codebook_size/2)][0] + perturbation;
            codebook[j][2] %= 256;
        }
        LGB(threshold,i,training_size,training,codebook,seed);
        
    }
    
    LGB(threshold,number_of_bits,training_size,training,codebook,seed);
    for (int i = 0; i < codebook_size; i++) {
        int index = codebook[i][0] + 256*codebook[i][1] + 65536*codebook[i][1];
        used_colors[index] = 1;
    }
    map < pair<int, pair<int,int> >, int > best_for_color; // mapa koloru na indeks codebooka
    double mse = 0;
    for (int i = 0; i <x; i++) {
        for (int j = 0; j < y; j++) {
  
                int a = arr1[i][j][0], b = arr1[i][j][1], c = arr1[i][j][2];
                map< pair<int, pair<int,int> >, int > ::iterator it = best_for_color.find(make_pair(a, make_pair(b, c)));
                if (it != best_for_color.end()) {
                    int id = it->second;
                    arr2[i][j][0] = codebook[id][0];
                    arr2[i][j][1] = codebook[id][1];
                    arr2[i][j][2] = codebook[id][2];
                    mse += d2(a,b,c,codebook[id][0],codebook[id][1],codebook[id][2]);
                }
                else if (used_colors[a + 256*b + 65536*c]) {
                    arr2[i][j][0] = a;
                    arr2[i][j][1] = b;
                    arr2[i][j][2] = c;
                }
                else {
                    int minimum = 1000000;
                    int minimum_id = 0;
                    for (int l = 0; l < codebook_size; l++) {
                        int dd = d_tax(a,b,c,codebook[l][0], codebook[l][1], codebook[l][2]);
                        if (dd < minimum) {
                            minimum_id = l;
                            minimum = dd;
                            if (dd == 1) {
                                break;
                            }
                        }
                    }
                    mse += d2(a,b,c,codebook[minimum_id][0],codebook[minimum_id][1],codebook[minimum_id][2]);
                    arr2[i][j][0] = codebook[minimum_id][0];
                    arr2[i][j][1] = codebook[minimum_id][1];
                    arr2[i][j][2] = codebook[minimum_id][2];
                    best_for_color[make_pair(a, make_pair(b, c))] = minimum_id;
                }
            
        }
        
    }
    mse /= x*y;
    cout << "Błąd średniokwadratowy: " << mse << "\n";
    double signal = 0;
    for (int i = 0; i <x; i++) {
        for (int j = 0; j < y; j++) {
            signal += arr1[i][j][0]*arr1[i][j][0] + arr1[i][j][1]*arr1[i][j][1] + arr1[i][j][2]*arr1[i][j][2];
        }
    }
    signal /= x*y;
    double snr = signal/mse;
    cout << "SNR: " << snr << "\n";
    cout << "SNR(dB): " <<  10.0*log10(snr) << "\n";

    return 0;
}
int write_tab_to_file(int*** arr, string file_name) {
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
int readBytes(string input) {
    int c = 0;
    FILE *fIN;
    fIN = fopen( input.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    
    
    auto size = filesystem::file_size(input.c_str());
    auto size1 = size;
    cout << "Rozmiar oryginalnego pliku w bajtach: " << size << endl;

    unsigned char z;
    bool endoffile = false;

    fread(header, 1, 18, fIN);
    y = int(header[12]);
    y += int(header[13]) * 256;
    cout <<"y: " << y << "\n";
    x = int(header[14]);
    x += int(header[15]) * 256;
    cout <<"x: " << x << "\n";

    bgr = new int**[x];
    for (int j = 0; j < x; j++) {
        bgr[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            bgr[j][i] = new int[3];
        }
    }
    bgr2 = new int**[x];
    for (int j = 0; j < x; j++) {
        bgr2[j] = new int*[y];
        for(int i = 0; i < y; i++) {
            bgr2[j][i] = new int[3];
        }
    }
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < 3; k++) {
                fread(&z, 1, 1, fIN);
                bgr[i][j][k] = z;
            }
        }
    }
    fclose(fIN);
    return 0;
}

int main(int argc, char** argv) {
    string input = argv[1];
    string output = argv[2];
    int number_of_bits = stoi(argv[3]);
    int training_size = stoi(argv[4]);
    int seed = stoi(argv[5]);
    readBytes(input);
    encode_tab(bgr,bgr2,number_of_bits,training_size,seed);
    write_tab_to_file(bgr2,output);
    

    
}