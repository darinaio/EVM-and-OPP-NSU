#include <stdio.h>
#include <time.h>
#define N 5900000000

double exp_maclaurin(double x) {
    double sum = 0.0;
    double term = 1;
    for (double n = 0; n < N; n++) {
        sum += term;
        term = term * x / (n + 1.0);
    }
    return sum;
}

int main() {
    double x;
    if(scanf("%lf", &x) !=1){
        printf("ERROR");
        return 0;
    }

    clock_t start_time = clock();
    double result = exp_maclaurin(x);
    clock_t end_time = clock();
    printf("%.10lf", (double)(end_time-start_time)/CLOCKS_PER_SEC);
    return 0;
}
