#include <stdio.h>
#include <stdint.h>

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    int j = (bits >> 52) - 1023; 
    int k = (bits >> (52 - j)) & 0x7;
    double cjk = (1.0 / (1ULL << j)) * (1.0 + (k >> 3));

    double R = x - cjk;

    double sin_R = ((0.000002755731922398589 * R + 
                     -0.0001984126984126984) * R + 
                     0.00833333333333333) * R * R + 
                     -0.1666666666666666 * R + 
                     1.0;

    return sin_R;
}

int main() {
    double x = M_PI / 8;
    double sin_R = sin_minimax(x);
    printf("sin_R: %.12lf\n", sin_R);

    return 0;
}