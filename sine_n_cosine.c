#include <stdio.h>
#include <stdint.h>

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    int j = (bits >> 52) - 1023; 
    int k = (bits >> (52 - j)) & 0x7;
    double cjk = (1.0 / (1ULL << j)) * (1.0 + (k >> 3));

    double R = x - cjk;

    double sin_R = ((((((-0.00018236 * x + 
                -0.0000273543) * x + 
                0.00835389) * x + 
                -8.15912e-6) * x + 
                -0.166665) * x + 
                -1.81057e-7) * x + 
                1.0) * x + 
                -6.42047e-11);

    return sin_R;
}

int main() {
    double x = M_PI / 8;
    double sin_R = sin_minimax(x);
    printf("sin_R: %.12lf\n", sin_R);

    return 0;
}