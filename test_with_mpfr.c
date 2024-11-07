#include <stdio.h>
#include <stdint.h>
#include <mpfr.h>

double table_sin_Cjk[7][4];
double table_cos_Cjk[7][4];

void init_tables() {
    int j, k;
    mpfr_t cjk, sin_cjk, cos_cjk;

    mpfr_init2(cjk, 256);
    mpfr_init2(sin_cjk, 256);
    mpfr_init2(cos_cjk, 256);
    for (j = -1; j >= -4; j--) {
        for (k = 1; k <= 7; k++) {
            int exponent = 1023 + j;
            uint64_t tmp = (uint64_t)exponent << 52;
            double cjk_value = (*(double*)&tmp) * (1.0 + (k * 0.125));

            mpfr_set_d(cjk, cjk_value, MPFR_RNDN);
            mpfr_sin(sin_cjk, cjk, MPFR_RNDN);
            mpfr_cos(cos_cjk, cjk, MPFR_RNDN);

            table_sin_Cjk[(-1)*j - 1][k - 1] = mpfr_get_d(sin_cjk, MPFR_RNDN);
            table_cos_Cjk[(-1)*j - 1][k - 1] = mpfr_get_d(cos_cjk, MPFR_RNDN);
        }
    }
}

void print_sin_table() {
    int i, j;
    printf("%10s", "X");
    for (j = 1; j <= 7; j++) {
        printf("%10d   ", j);
    }
    printf("\n");
    for (i = 1; i <= 4; i++) {
        printf("%10d   ", i);
        for (j = 1; j <= 7; j++) {
            printf("%10lf   ", table_sin_Cjk[i][j]);
        }
        printf("\n");
    }
}

void print_cos_table() {
    int i, j;
    printf("%10s", "X");
    for (j = 1; j <= 7; j++) {
        printf("%10d   ", j);
    }
    printf("\n");
    for (i = 1; i <= 4; i++) {
        printf("%10d   ", i);
        for (j = 1; j <= 7; j++) {
            printf("%10lf   ", table_cos_Cjk[i][j]);
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

    double cos_R = 0x1p0 + x * 
    (0x1.9a90471eedea2p-101 + x * 
    (-0x1p-1 + x * 
    (-0x1.c35565375f36bp-89 + x * 
    (0x1.5555555555555p-5 + x * 
    (0x1.ff3d9ead9207dp-79 + x * 
    (-0x1.6c16c16c13c9cp-10 + x * 
    (-0x1.a18e77efe123p-70 + x * 
    (0x1.a01a008ec3853p-16 + x * 
    (0x1.be000f0503974p-63 + x * 
    (-0x1.27c32619713b7p-22))))))))));

    double result = table_sin_Cjk[(-1)*j - 1][k - 1] * cos_R + 
                        table_cos_Cjk[(-1)*j - 1][k - 1] * sin_R;

    return result;

    return sin_R;
}

int main() {
    mpfr_t angle_pi_16, angle_pi_32, sin_res_16, sin_res_32;
    double Pi = 3.14159265358979323846;
    double pi_16 = Pi / 16, pi_32 = Pi / 32;
    double res_pi_16, res_pi_32;
    
    init_tables();

    // Инициализация переменных с точностью 256 бит
    mpfr_init2(angle_pi_16, 256);
    mpfr_init2(angle_pi_32, 256);
    mpfr_init2(sin_res_16, 256);
    mpfr_init2(sin_res_32, 256);

    // Установка значения угла (например, π/4)
    mpfr_const_pi(angle_pi_16, MPFR_RNDN);
    mpfr_div_ui(angle_pi_16, angle_pi_16, 16, MPFR_RNDN);

    mpfr_const_pi(angle_pi_32, MPFR_RNDN);
    mpfr_div_ui(angle_pi_32, angle_pi_32, 32, MPFR_RNDN);

    // Вычисление синуса и косинуса
    mpfr_sin(sin_res_16, angle_pi_16, MPFR_RNDN);
    mpfr_sin(sin_res_32, angle_pi_32, MPFR_RNDN);

    res_pi_16 = sin_minimax(pi_16);
    res_pi_32 = sin_minimax(pi_32);

    // Вывод результатов
    printf("MPFR: sin(π/16) = ");
    mpfr_printf("%.25Rf\n", sin_res_16);
    printf("MY_FUNC: sin(π/16) = ");
    printf("%.25f\n", res_pi_16);

    printf("MPFR: sin(π/32) = ");
    mpfr_printf("%.25Rf\n", sin_res_32);
    printf("MY_FUNC: sin(π/32) = ");
    printf("%.25f\n", res_pi_32);

    // Очистка памяти
    mpfr_clear(angle_pi_16);
    mpfr_clear(angle_pi_32);
    mpfr_clear(sin_res_16);
    mpfr_clear(sin_res_32);
    mpfr_free_cache();

    return 0;
}