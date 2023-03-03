#include <iostream>
#include <string>
#include <bitset>
#include <filesystem>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

int current_bit = 0;
unsigned char bit_buffer;
const int andValue = 65535;


vector<double> len;
long long occurances[256];
long long L[256];
double P[256];

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
    fwrite (&bit_buffer, 1, 1, f);
    current_bit = 0;
    bit_buffer = 0;
  }
}
void flushBits (void)
{
  while (current_bit)
    writeBit (0);
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
int trimMSBitsLong(long long *x,int curr, int n, int maxBit) {
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
bool E3(long long a, long long b, int m) {
    if ((1 << (m-1)) + (1 << (m-2)) > b && (1 << (m-2)) <= a) {
        return true;
    }
    return false;
}
void process_arguments(string& in, string& out, const char* input,const char* output) {
    if(!output) {
        if (input) {
            out = string(input) + ".enc";
        }
        else {
            out = "out.enc";
        }
    }
    else {
        out = string(output);
    }
    if (!input) {
        in = "in.in";
    }
    else {
        in = string(input);
    }
}

int encode(int* n,const char* input,const char* output, int m) {

    string in,out;
    process_arguments(in,out,input,output);
    
    int* update = new int[256];
    int update_count = 0;
    for (int i =0; i < 256; i++) {
        update[i] = 0;
    }
    fIN = fopen( in.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    f = fopen( out.c_str(), "wb" );
    if (!f) {
        cerr << "Nie ma takiego pliku.\n";
        return 1;
    }
    
    auto size = filesystem::file_size(in.c_str());
    auto size1 = size;
    cout << "Rozmiar oryginalnego pliku w bajtach: " << size << endl;
    
    while (size != 0) {
        writeBit(size & 1);
        size = size >> 1;
    }
    for (int i =0; i < 64; i++) {
        writeBit(1);
    }
    writeBit(0);
    int mostsignificant =1;
    mostsignificant = mostsignificant << (m-1);
    int* cum_count = new int[257];
    cum_count[0] = 0;
    for (int i = 1; i <= 256; i++) {
        cum_count[i] = cum_count[i-1] + n[i-1];
    }
    int total_count = cum_count[256];
    long long l = 0;
    long long u = 1;
    int Scale3 = 0;
    for (int i = 0; i < m-1; i++) {
        u = u << 1;
        u += 1;
    }
    unsigned char x;
    while (fread(&x, 1, 1, fIN) == 1) {
        occurances[x]++;
        L[x]++;
        long long lold = l;
        l = l + ((u-l+1)*cum_count[x])/total_count;
        u = lold + ((u-lold+1)*cum_count[x+1])/total_count - 1;
        while ((u & mostsignificant) == (l & mostsignificant) ||  E3(l,u,m)) {
            if ((u & mostsignificant) == (l & mostsignificant)) {
                int b;
                if (u & mostsignificant) {
                    b = 1;
                    writeBit(b);
                }
                else {
                    b = 0;
                    writeBit(b);
                }

                l = l << 1;
                u = u << 1;
                u += 1;
                u = u & andValue;
                l = l & andValue;
                while (Scale3 > 0) {
                    writeBit(b ^ 1);
                    Scale3--;
                }
            }
            if (E3(l,u,m)) {
                l = l << 1;
                u = u << 1;
                u += 1;
                u = u & andValue;
                l = l & andValue;
                u = u ^ mostsignificant;
                l = l ^ mostsignificant;
                Scale3++;
            }
        }
        update[x]++;
        update_count++;
        if (update_count == 256) {

            double avg_length = 0;
            for (int i= 0; i < 256; i++) {
                avg_length += - (log2( (double(n[i])/total_count) )*(double(occurances[i])/256.0));
            }
            len.push_back(avg_length);

            update_count=0;
            for (int i= 0; i < 256; i++) {
                occurances[i] = 0;
                n[i] += update[i];
                update[i] = 0;
            }
            if (total_count > andValue/4) {
                for (int i = 0; i < 256; i++) {
                    if (n[i] > 1) {
                        n[i] = n[i]/2;
                    }
                }
            }
            cum_count[0] = 0;
            for (int i= 1; i <= 256; i++) {
                cum_count[i] = cum_count[i-1] + n[i-1];
            }
            total_count = cum_count[256];
        }
    }
    int a = trimMSBitsLong(&l,0,1,m-1);
    writeBit(a);
    for (int i = 0; i < Scale3; i++) {
        if (a)
            writeBit(0);
        else
            writeBit(1);
    }
    for (int i =1; i < m; i++) {
        writeBit(trimMSBitsLong(&l,i,1,m-1));
    }
    flushBits();
    fclose(f);
    fclose(fIN);
    auto size2 = filesystem::file_size(out.c_str());
    cout << "Zapisano do pliku " << out << '\n';
    cout << "Rozmiar zakodowanego pliku w bajtach: " << size2 << endl;
    cout << "(" << (double(size2)*100.0/size1) << "\% oryginalnego rozmiaru)\n";
    for (int i= 0; i < 256; i++) {
        P[i] = double(L[i])/size1;
    }
    return 0;

}
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}
int main(int argc, char * argv[]) {
    char * input = getCmdOption(argv, argv + argc, "-i");
    char * output = getCmdOption(argv, argv + argc, "-o");
    int* n = new int[256];
    for (int i = 0; i <= 255; i++) {
        occurances[i] = 0;
        n[i] =1;
    }
    int enc = (encode(n,input,output,16));
    if (enc) {
        cerr << "Wystapil blad.\n";
        return 1;
    }
    double avg_len = 0;
    for (int i =0; i < len.size(); i++) {
        avg_len += len[i]/len.size();
    }
    double entropia = 0;
    for (int i = 0; i <= 255; i++) {
        if (L[i]) {
            entropia+= P[i]*(-log2(P[i]));
        }
    }
    cout << "Srednia dlugosc kodowania: " << avg_len << "\n";
    cout << "Entropia: " << entropia << "\n";
    cout << "Srednia dlugosc kodowania - Entropia: " << avg_len - entropia << "\n";


    return 0;
}