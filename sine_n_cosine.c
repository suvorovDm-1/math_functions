#include <stdio.h>
#include <stdint.h>

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    int j = (bits >> 52) - 1023; 
    int k = (bits >> (52 - j)) & 0x7;
    int exponent = 1023 - j;
    uint64_t tmp = (uint64_t)exponent << 52;
    double cjk = (*(double*)&tmp) * (1.0 + (k >> 3));

    double R = x - cjk;

    // Wolfram coeffs:
    double sin_R = ((((((-0.00018236 * R + 
                -0.0000273543) * R + 
                0.00835389) * R + 
                -8.15912e-6) * R + 
                -0.166665) * R + 
                -1.81057e-7) * R + 
                1.0) * R + 
                -6.42047e-11;

    double cos_R = ((((-0.00127554 * R + 
                   -0.000163694) * R + 
                   0.0417661) * R + 
                   -0.0000296826) * R + 
                   -0.499996) * R + 
                   -2.28712e-7;

    return sin_R;
}

int main() {
    double x = M_PI / 8;
    double sin_R = sin_minimax(x);
    printf("sin_R: %.12lf\n", sin_R);

    return 0;
}