#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int K = 10;
const int N_MIN = 256;                 //1Kb
const int N_MAX = 4 * 1024 * 1024;    //16Mb
const int COUNT = 50;

void swap(int *a, int *b);
void randomize_fisher_yates(int *arr, int n);
void create_array(int *array, int N);
void create_reversed_array(int *array, int N);
void create_randomized_array(int *array, int N);

long double tacts(int *arr, int N);

int main() {
    FILE *out = fopen("resultO1.csv", "w");
    fprintf(out, "Size;Forward;Reverse;Random\n");

    for (int N = N_MIN; N <= N_MAX; N = (int)(N * 1.2)) {
        int *array = (int *)calloc(N, sizeof(int));
        if (array == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(1);
        }

        create_array(array, N);
        long double time_forward = tacts(array, N);

        create_reversed_array(array, N);
        long double time_reverse = tacts(array, N);

        create_randomized_array(array, N);
        long double time_random = tacts(array, N);

        fprintf(out, "%ld;%Lf;%Lf;%Lf\n", N * sizeof(int), time_forward, time_reverse, time_random);
        free(array);
    }
    fclose(out);
    return 0;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize_fisher_yates(int *arr, int n) {
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&arr[i], &arr[j]);
    }
}

void create_array(int *array, int N) {
    for (int i = 0; i < N - 1; i++) {
        array[i] = i + 1;
    }
    array[N - 1] = 0;
}

void create_reversed_array(int *array, int N) {
    array[0] = N - 1;
    for (int i = 1; i < N; i++) {
        array[i] = i - 1;
    }
}

void create_randomized_array(int *array, int N) {
    create_array(array, N);
    randomize_fisher_yates(array, N);
}

long double tacts(int *arr, int N) {
    long double min_avg = 1e18; // Большое значение для инициализации
    for (int it = 0; it < COUNT; ++it) {
        volatile int x = 0;
        for (int i = 0; i < N; ++i) {
            x = arr[x];
        }

        unsigned long long start, end;
        start = __builtin_ia32_rdtsc();

        for (int i = 0; i < N * K; ++i) {
            x = arr[x];
        }

        end = __builtin_ia32_rdtsc();

        long double avg = (long double)(end - start) / (N * K);
        if (avg < min_avg) min_avg = avg;
    }
    return min_avg;
}

