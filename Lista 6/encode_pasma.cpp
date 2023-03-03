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
int*** pasmo_srednie;
int*** pasmo_roznica;
int*** pasmo_srednie_enc;
int*** pasmo_roznica_enc;
int x;
int y;

unsigned char bit_buffer;
int current_bit = 0;
int fib[46];
FILE *f;
FILE *fIN;


void writeBit (int bit)
{
    bit_buffer = bit_buffer << 1;
    if (bit) {
        bit_buffer |= 1;
    }

  current_bit++;
  if (current_bit == 8)
  {
    std::fwrite (&bit_buffer, 1, 1, f);
    current_bit = 0;
    bit_buffer = 0;
  }
}
void flushBits (int x)
{
  while (current_bit)
    writeBit (x);
}


int getval(int*** arr, int a, int b, int c) {
    if (0 <=a && a < x && 0 <= b && b < y) {
        return arr[a][b][c];
    }
    return 0;
}
int length(int in) {
    unsigned bits, var = (in < 0) ? -in : in;
    for(bits = 0; var != 0; ++bits) var >>= 1;
    return bits;
}


vector<int> generate_geometric(int n) {
    vector<int> values;
    if (n == 2) {
        values.push_back(-1);
        values.push_back(1);
    }
    else {
        double inc = 8.0/double(n/2-1);
        double curr = 0;
        for (int i = 0; i < n/2-1; i++) {
            curr += inc;
            values.push_back(-min(255,int(pow(2,curr))));
            
            //cout << curr << " ";
        }
        curr = 0;
        for (int i = 0; i < n/2-1; i++) {
            curr += inc;
            values.push_back(min(255,int(pow(2,curr))));
            
            //cout << curr << " ";
        }
        /*for (int i = 0; i < n; i++) {
            std::cout << values[i] << ", ";
        }*/
        std::cout << "\n";
        sort( values.begin(), values.end() );
        values.erase( unique( values.begin(), values.end() ), values.end() );
        int start1 = n/2;
        int start2 = n/2-1;
        while (values.size() < n) {
            for (int i = start1; i < values.size(); i++) {
                if (values[i] > values[i-1]+1) {
                    //cout << "v " << values[i] << "\n";
                    values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                    start1=i;
                    break;
                }
            }
            if (values.size() < n) {
                for (int i = start2; i < values.size(); i--) {
                    if (values[i] > values[i-1]+1) {
                        values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                        start2= i;
                        break;
                    }
                }
            }
            sort( values.begin(), values.end() );
        }
        for (int i = 0; i < n; i++) {
            std::cout << values[i] << ", ";
        }
        std::cout << "\n";
    }
    return values;
}
vector<int> generate_uniform(int n) {
    
    vector<int> values;
    if (n == 2) {
        values.push_back(-128);values.push_back(128);
    }
    else {
        int curr = 0;
        int inc = 512/(n-2);
        //values.push_back(0);values.push_back(1);values.push_back(2);values.push_back(-1);values.push_back(-2);values.push_back(4);
        for (int i = 0; i< n/2-1; i++) {
            curr += inc;
            values.push_back(-curr);
            values.push_back(curr);
        }
        sort( values.begin(), values.end() );
    }
    //cout << "debug " << values.size();
    values.erase( unique( values.begin(), values.end() ), values.end() );
    int start1 = n/2;
    int start2 = n/2-1;
    while (values.size() < n) {
        for (int i = start1; i < values.size(); i++) {
            if (values[i] > values[i-1]+1) {
                //cout << "v " << values[i] << "\n";
                values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                start1=i;
                break;
            }
        }
        if (values.size() < n) {
            for (int i = start2; i < values.size(); i--) {
                if (values[i] > values[i-1]+1) {
                    values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                    start2= i;
                    break;
                }
            }
        }
        sort( values.begin(), values.end() );
    }
    for (int i = 0; i < n; i++) {
        std::cout << values[i] << ", ";
    }
    std::cout << "\n";
    return values;

}
vector<int> generate_compressed(int n) {
    vector<int> values = generate_uniform(n);
    for (int i =0; i < n; i++) {
        if (-64 <= values[i] && values[i] <= 64) {
            values[i]*=2;
        }
        else if (values[i] > 64){
            values[i] = 2.0*values[i]/3 + 85;
        }
        else {
            values[i] = 2.0*values[i]/3 - 85;
        }
    }
    values.erase( unique( values.begin(), values.end() ), values.end() );
    sort( values.begin(), values.end() );
    int start1 = n/2;
    int start2 = n/2-1;
    while (values.size() < n) {
        for (int i = start1; i < values.size(); i++) {
            if (values[i] > values[i-1]+1) {
                //cout << "v " << values[i] << "\n";
                values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                start1=i;
                break;
            }
        }
        if (values.size() < n) {
            for (int i = start2; i < values.size(); i--) {
                if (values[i] > values[i-1]+1) {
                    values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                    start2= i;
                    break;
                }
            }
        }
        sort( values.begin(), values.end() );
    }
    
    for (int i = 0; i < n; i++) {
        std::cout << values[i] << ", ";
    }
    return values;
}
vector<int> generate_expanded(int n) {
    vector<int> values = generate_uniform(n);
    for (int i =0; i < n; i++) {
        if (-128 <= values[i] && values[i] <= 128) {
            values[i]= values[i]/2;
        }
        else if (values[i] > 128){
            values[i] = 3.0*values[i]/2 - 128;
        }
        else {
            values[i] = 3.0*values[i]/2 + 128;
        }
    }
    values.erase( unique( values.begin(), values.end() ), values.end() );
    int start1 = n/2;
    int start2 = n/2-1;
    while (values.size() < n) {
        for (int i = start1; i < values.size(); i++) {
            if (values[i] > values[i-1]+1) {
                //cout << "v " << values[i] << "\n";
                values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                start1=i;
                break;
            }
        }
        if (values.size() < n) {
            for (int i = start2; i < values.size(); i--) {
                if (values[i] > values[i-1]+1) {
                    values.insert(values.begin() + i, (values[i-1]+values[i])/2);
                    start2= i;
                    break;
                }
            }
        }
        sort( values.begin(), values.end() );
    }
    
    for (int i = 0; i < n; i++) {
        std::cout << values[i] << ", ";
    }
    return values;
}

int write_tab_to_file(int*** pasmo_r, int*** pasmo_s, vector<int> values_sre, vector<int> values_roz, string file_name, int numberofbits) {
    f = fopen( file_name.c_str(), "wb" );
    if (!f) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    int t = numberofbits;
    for (int i =0; i < length(numberofbits); i++) {
        std::cout << t%2 << "\n";
        writeBit(t%2);
        writeBit(t%2);
        t/=2;
    }
    writeBit(0);
    writeBit(1);
    
    flushBits(0);
    for (int i = 0; i < values_sre.size(); i++) {
        unsigned char c = (unsigned char)(abs(values_sre[i]));
        std::fwrite(&c,1,1,f);
    }
    for (int i = 0; i < values_roz.size(); i++) {
        unsigned char c = (unsigned char)(abs(values_roz[i]));
        std::fwrite(&c,1,1,f);
    }
    std::fwrite(header,1,18,f);

    for (int k = 0; k < 3; k++) {
        for (int i = 0; i <x; i++) {
            for (int j = 0; j < y; j+=2) {
                    int t = pasmo_r[k][i][j];
                    for (int l =0; l < numberofbits; l++) {
                        
                        if (t % 2 == 1) {
                            writeBit(1);
                        }
                        else {
                            writeBit(0);
                        }
                        t /= 2;
                        
                    }
                    //cout << k << " " << i << " " << j << " roznica " << values[pasmo_r[k][i][j]] << "\n";
                }
        }
    }
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i <x; i++) {
            for (int j = 0; j < y; j+=2) {
                    int t = pasmo_s[k][i][j];
                    for (int l =0; l < numberofbits; l++) {
                        
                        if (t % 2 == 1) {
                            writeBit(1);
                        }
                        else {
                            writeBit(0);
                        }
                        t /= 2;
                    }
                    //cout << k << " " << i << " " << j << " srednia " << values[pasmo_s[k][i][j]] << ", " << pasmo_s[k][i][j] << "\n";
                }
                
        }
    }
    flushBits(0);
    std::fwrite(&bgr[0][0][0],1,1,f);
    std::fwrite(&bgr[0][0][1],1,1,f);
    std::fwrite(&bgr[0][0][2],1,1,f);
    return 0;
}
int encode_tab(int*** p_srednie, int*** p_roznica, int*** p_srednie2, int*** p_roznica2, int number_of_bits, string output, string sre_type, string roz_type) {
    cout << "generacja slownika...\n"; 
    vector<int> values_sre;
    vector<int> values_roz;
    if (sre_type == "geometric") {
    values_sre = generate_geometric(pow(2,number_of_bits));
    }
    else if (sre_type == "uniform") {
        values_sre = generate_uniform(pow(2,number_of_bits));
    }
    else if (sre_type == "compressed") {
        values_sre = generate_compressed(pow(2,number_of_bits));
    }
    else if (sre_type == "expanded") {
        values_sre = generate_expanded(pow(2,number_of_bits));
    }

    if (roz_type == "geometric") {
    values_roz = generate_geometric(pow(2,number_of_bits));
    }
    else if (roz_type == "uniform") {
        values_roz = generate_uniform(pow(2,number_of_bits));
    }
    else if (roz_type == "compressed") {
        values_roz = generate_compressed(pow(2,number_of_bits));
    }
    else if (roz_type == "expanded") {
        values_roz = generate_expanded(pow(2,number_of_bits));
    }

    cout << "wielkosc:" << values_sre.size() << "\n";
    //kodowanie roznicowe
    int*** decoded_x = new int**[3];
    for (int j = 0; j < 3; j++) {
        decoded_x[j] = new int*[x];
        for(int i = 0; i < x; i++) {
            decoded_x[j][i] = new int[y];
        }
    }
    for (int k = 0; k < 3; k++) {
        decoded_x[k][0][0] = p_srednie[k][0][0];
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j+=2) {
                
                //cout << "srednie0: " << p_srednie[k][i][j] << "\n";
                if (j > 1) {
                    p_srednie2[k][i][j] = (p_srednie[k][i][j] - decoded_x[k][i][j-2]);// + 255)%255;
                }
                else if (i > 0) {
                    p_srednie2[k][i][j] = (p_srednie[k][i][j] - decoded_x[k][i-1][j]);// + 255)%255;
                }
                else {
                    p_srednie2[k][i][j] = p_srednie[k][i][j];
                }
                bool srednie_changed = false;
                //cout << "srednie preenc " << p_srednie2[k][i][j] << "\n";
                for (int l = 0; l < values_sre.size()-1; l++) {
                    //std::cout << i <<  " " << j << "\n" << flush;
                    if (!srednie_changed && values_sre[l] <= p_srednie2[k][i][j] && values_sre[l+1] >= p_srednie2[k][i][j]) {
                        srednie_changed = true;
                        if (abs(p_srednie2[k][i][j]-values_sre[l+1]) > abs(p_srednie2[k][i][j]-values_sre[l]) ) {
                            p_srednie2[k][i][j] = l;
                        }
                        else {
                            p_srednie2[k][i][j] = l+1;
                        }
                    }
                }
                if (!srednie_changed) {
                    p_srednie2[k][i][j] = values_sre.size()-1;
                }
                if (j > 1) {
                    decoded_x[k][i][j] = decoded_x[k][i][j-2] + values_sre[p_srednie2[k][i][j]];
                }
                else if (i > 0) {
                    decoded_x[k][i][j] = decoded_x[k][i-1][j] + values_sre[p_srednie2[k][i][j]];
                }
                //decoded_x[k][i][j] = min(decoded_x[k][i][j],255);
                //decoded_x[k][i][j] = max(decoded_x[k][i][j],0);
                /*if (abs(p_srednie[k][i][j]-decoded_x[k][i][j] > 5)){
                    cout << "oryginal: " << p_srednie[k][i][j] << "\n";
                    cout << "decoded: " << decoded_x[k][i][j] << "\n";
                    cout << "codebook val << "  << i << ", " << j << ", " << p_srednie2[k][i][j]  << "\n";
                }*/
            }
        }
    }
    //kwantyzacja
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j+=2) {
                bool roznice_changed = false;
                p_roznica2[k][i][j] = values_roz.size()-1;
                //cout << "srednie preenc " << p_srednie2[k][i][j] << "\n";
                for (int l = 0; l < values_roz.size()-1; l++) {
                    //std::cout << i <<  " " << j << "\n" << flush;
                    
                    if (!roznice_changed &&values_roz[l] <= p_roznica[k][i][j] && values_roz[l+1] >= p_roznica[k][i][j]) {
                        roznice_changed = true;
                        if (abs(p_roznica[k][i][j]-values_roz[l+1]) > abs(p_roznica[k][i][j]-values_roz[l]) ) {
                            p_roznica2[k][i][j] = l;
                        }
                        else {
                            p_roznica2[k][i][j] = l+1;
                        }
                    }
                }
                //cout << "srednie enc " << p_srednie2[k][i][j] << "\n";
            }
        }
    }
    write_tab_to_file(p_roznica2,p_srednie2,values_sre, values_roz,output,number_of_bits);

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

    fread(header, 1, 18, fIN);
    y = int(header[12]);
    y += int(header[13]) * 256;
    std::cout <<"y: " << y << "\n";
    x = int(header[14]);
    x += int(header[15]) * 256;
    std::cout <<"x: " << x << "\n";

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
    pasmo_roznica_enc = new int**[3];
    pasmo_srednie_enc = new int**[3];
    for (int j = 0; j < 3; j++) {
        pasmo_roznica_enc[j] = new int*[x];
        pasmo_srednie_enc[j] = new int*[x];
        for(int i = 0; i < x; i++) {
            pasmo_roznica_enc[j][i] = new int[y];
            pasmo_srednie_enc[j][i] = new int[y];
        }
    }
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < 3; k++) {
                //std::cout << i << " " << j << "\n" << flush;
                fread(&z, 1, 1, fIN);
                bgr[i][j][k] = z;
                if (j != 0) {
                    pasmo_roznica[k][i][j] = (z - bgr[i][j-1][k])/2;
                    pasmo_srednie[k][i][j] = (z + bgr[i][j-1][k])/2;
                }
                else if (j != 0) {
                    pasmo_roznica[k][i][j] = (z - bgr[i-1][j][k])/2;
                    pasmo_srednie[k][i][j] = (z + bgr[i-1][j][k])/2;
                }
                else {
                    pasmo_roznica[k][i][j] = 0;
                    pasmo_srednie[k][i][j] = z;
                }
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
    string sre_type = argv[4];
    string roz_type = argv[5];
    readBytes(input);
    encode_tab(pasmo_srednie,pasmo_roznica,pasmo_roznica_enc,pasmo_srednie_enc, number_of_bits,output, sre_type, roz_type);
    //write_tab_to_file(bgr2,output);
    

    
}