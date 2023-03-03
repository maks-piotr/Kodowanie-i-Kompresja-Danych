#include <iostream>
#include <string>
#include <bitset>
#include <filesystem>
#include <algorithm>
using namespace std;

int current_bit = 0;
unsigned char bit_buffer;
const int andValue = 65535;

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
int read_bits(int n) {
    int count=0;
    int solution=0;
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
        bit_buffer = (unsigned char)(fgetc(fIN));
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
bool E3(long long a, long long b, int m) {
    if ((1 << (m-1)) + (1 << (m-2)) > b && (1 << (m-2)) <= a) {
        return true;
    }
    return false;
}
void process_arguments(string& in, string& out, const char* input,const char* output) {
    if(!output) {
        if (input) {
            if (string(input).find("/") == -1 && string(input).find("\\") == -1) {
                if (string(input).find(".enc") != -1) {
                    string noenc = string(input).substr(0, string(input).find(".enc"));
                    out = "decoded_" + noenc;
                }
                else
                    out = "decoded_" + string(input);
            }
            else if (string(input).find("/") != -1) {
                int occ = string(input).find_last_of('/');
                out = string(input).substr(0,occ+1) + "decoded_" + string(input).substr(occ+1, string(input).size());
                if (string(input).find(".enc") != -1) {
                    string noenc = out.substr(0, out.find(".enc"));
                    out = noenc;
                }
            }
            else if (string(input).find("\\") != -1) {
                int occ = string(input).find_last_of('\\');
                out = string(input).substr(0,occ+1) + "decoded_" + string(input).substr(occ+1, string(input).size());
                if (string(input).find(".enc") != -1) {
                    string noenc = out.substr(0, out.find(".enc"));
                    out = noenc;
                }
            }
        }
        else {
            out = "decoded_in.in";
        }
    }
    else {
        out = string(output);
    }
    if (!input) {
        in = "out.enc";
    }
    else {
        in = string(input);
    }
}


int decode(int* n, const char* input,const char* output,  int m) {
    string in,out;
    process_arguments(in,out,input,output);
    fIN = fopen( in.c_str(), "rb" );
    if (!fIN) {
        cerr << "Nie ma takiego pliku. (" << in << ")\n";
        return 1;
    }
    f = fopen( out.c_str(), "wb" );
    if (!f) {
        cerr << "Nie ma takiego pliku. (" << out << ")\n";
        return 1;
    }
    auto size = filesystem::file_size(in.c_str());
    cout << "Rozmiar zakodowanego pliku w bajtach: " << size << endl;
    int* update = new int[256];
    int update_count = 0;
    for (int i =0; i < 256; i++) {
        update[i] = 0;
    }
    int index = 0;
    int ones_counter=0;
    bitset<128> size_intake;
    while (true) {
        int bit = read_bits(1);
        if (ones_counter >= 64 && bit == 0) {
            break;
        }
        size_intake.set(index,bit);
        index++;
        if (bit) {
            ones_counter++;
        }
        else {
            ones_counter = 0;
        }
    }
    for (int i = index-1; i > index-65; i--) {
        size_intake.set(i,0);
    }
    int symbol_count = size_intake.to_ullong();
    cout << "Rozmiar oryginalnego pliku w bajtach: " << symbol_count << "\n";

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
    unsigned char x;
    for (int i = 0; i < m-1; i++) {
        u = u << 1;
        u += 1;
    }
    long long t = read_bits(m);

    while (symbol_count > 0) {
        symbol_count--;
        int k = 0;
        while (((t-l+1)*total_count-1)/(u-l+1) >= cum_count[k+1]) {
            k++;
        }
        x = k;
        fwrite(&x, 1, sizeof(unsigned char), f);
        long long lold = l;
        l = l + (u-l+1)*cum_count[x]/total_count;
        u = lold + ((u-lold+1)*cum_count[x+1])/total_count - 1;
        while ((u & mostsignificant) == (l & mostsignificant) ||  E3(l,u,m)) {
            if ((u & mostsignificant) == (l & mostsignificant)) {

                l = l << 1;
                u = u << 1;
                u += 1;
                t = t << 1;
                t += read_bits(1);
                l = l & andValue;
                u = u & andValue;
                t = t & andValue;
            }
            if (E3(l,u,m)) {
                l = l << 1;
                u = u << 1;
                u += 1;
                t = t << 1;
                t += read_bits(1);
                l = l & andValue;
                u = u & andValue;
                t = t & andValue;
                u = u ^ mostsignificant;
                l = l ^ mostsignificant;
                t = t ^ mostsignificant;
            }
        }
        update[x]++;
        update_count++;
        if (update_count == 256) {
            update_count=0;
            for (int i= 0; i < 256; i++) {
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
    fclose(f);
    fclose(fIN);
    cout << "Zapisano zdekodowany plik do " << out << "\n";
    return 0;
}
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
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
        n[i] =1;
    }
    return decode(n,input, output, 16);
}