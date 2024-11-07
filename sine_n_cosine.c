#include <stdio.h>
#include <stdint.h>
#include <mpfr.h>

double table_Cjk[7][4];

void init_table() {
    int j, k;
    for (j = -1; j >= -4; j--) {
        for (k = 1; k <= 7; k++) {
            int exponent = 1023 + j;
            uint64_t tmp = (uint64_t)exponent << 52;
            double cjk = (*(double*)&tmp) * (1.0 + (k * 0.125));

            table_Cjk[(-1)*j - 1][k - 1] = cjk;
        }
    }
}

void print_table() {
    int i, j;
    printf("%10s", "X");
    for (j = 1; j <= 7; j++) {
        printf("%10d   ", j);
    }
    printf("\n");
    for (i = 1; i <= 4; i++) {
        printf("%10d   ", i);
        for (j = 1; j <= 7; j++) {
            printf("%10lf   ", table_Cjk[i][j]);
        }
        printf("\n");
    }
}

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    uint64_t sign_mask = 0x7FFFFFFFFFFFFFFF;
    bits &= sign_mask;

    int j = (bits >> 52) - 1023;
    if (j < -4) j = -4;
    if (j > -1) j = -1;

    uint64_t mantissa = bits & 0xFFFFFFFFFFFFF;
    int k = (mantissa >> (52 + j)) & 0x7;
    if (k == 0) k = 1;

    int exponent = 1023 + j;
    uint64_t tmp = (uint64_t)exponent << 52;
    double cjk = (*(double*)&tmp) * (1.0 + (k * 0.125));

    printf("x:%f j:%d k:%d cjk:%f", x, j, k, cjk);

    double R = x - cjk;

    double sin_R = 0x1.c5f0261f1868cp-95 + R * 
    (0x1p0 + R * (-0x1.f41bcf0494c2ap-81 + R * 
    (-0x1.5555555555555p-3 + R * 
    (0x1.f93211fd72046p-70 + R * 
    (0x1.111111110e667p-7 + R * 
    (-0x1.59e56126ed6f9p-60 + R * 
    (-0x1.a019ffd908d95p-13 + R * 
    (0x1.7e72a473c5d21p-52 + R * 
    (0x1.71621f6a299f4p-19 + R * 
    (-0x1.268826342f202p-45 + R * 
    0x1.62f40c5e99247p-22))))))))));

    return R;
}

int main() {
    double x = 3.14159265358979323846 / 16;
    double sin_R = sin_minimax(x);
    printf("sin_R: %.12lf\n", sin_R);

    init_table();
    print_table();

    return 0;
}