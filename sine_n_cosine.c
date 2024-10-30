#include <stdio.h>
#include <math.h>
#include <stdint.h>

const double sin_cjk_table[8] = {
    0.0, 0.12499999999999995, 0.24999999999999994, 0.3749999999999999,
    0.49999999999999994, 0.6249999999999999, 0.7499999999999999,
    0.8749999999999999
};

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    int j = (bits >> 52) - 1023;
    int k = (bits >> (52 - j)) & 0x7;
    double cjk = (1.0 / pow(2, j)) * (1.0 + (double)k / 8.0);
    double R = x - cjk;

    double R_squared = R * R;

    double sin_R = 0.9999999999999998 + 
                  -0.1666666666666666 * R + 
                  0.00833333333333333 * R_squared + 
                  -0.0001984126984126984 * R * R_squared + 
                  0.000002755731922398589 * R * R_squared * R;

    int index = k; 
    double sin_cjk = sin_cjk_table[index]; 

    printf("R: %.12lf\n", R);
    printf("sin(R): %.12lf\n", sin_R);
    printf("sin(cjk): %.12lf\n", sin_cjk);

    return sin_R;
}


int main() {
    double x = M_PI / 6;
    double sin_R = sin_minimax(x);
    printf("sin_R: %.12lf\n", sin_R);

    return 0;
}