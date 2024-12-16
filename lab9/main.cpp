#include <iostream>
#include <vector>
#include <fstream>

int main() {
    unsigned offset = 12 * 1024 * 1024 / sizeof(unsigned );
    unsigned cache_size = (32 * 1024 + 256 * 1024 + 3 * 1024 * 1024) / sizeof(unsigned );
    unsigned k = 1;
    std::ofstream out;
    out.open("out.csv", std::ios::out);
    for(unsigned n = 1; n < 33; n++){
        unsigned size = offset * n;
        std::vector<unsigned > array(size);
        for(unsigned i = 0; i < n - 1; i++){
            for(unsigned j = 0; j < cache_size / n; j++){
                array[i * offset + j] = (i + 1) * offset + j;
            }
        }
        for(unsigned i = 0; i < cache_size / n; i++){
            array[(n - 1) * offset + i] = (i + 1) % (cache_size / n);
        }
        volatile unsigned index = 0;
        for (int i = 0; i < cache_size; i++) { //прогрев кэша
            index = array[index];
        }

        unsigned long long start, end;
        unsigned long long minTicks = -1;
        for(size_t j = 0; j < k; j++) {
            asm volatile ("rdtsc" : "=A"(start));

            volatile unsigned index = 0;
            for (int i = 0; i < cache_size; i++) {
                index = array[index];
            }
            asm volatile ("rdtsc" : "=A"(end));
            unsigned long long cycles = end - start;

            if(minTicks > cycles){
                minTicks = cycles;
            }
        }
        out << n << "\t" << minTicks / cache_size << std::endl;
    }
    return 0;
}