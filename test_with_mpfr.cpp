#include <string.h>
#include <stdio.h>
#include <random>
#include <mpfr.h>
#include <cmath>
#include <cstdint>

double table_sin_Cjk[7][4];
double table_cos_Cjk[7][4];

// uint64_t difference_in_bits(double mpfr_result, double my_result) {
//     uint64_t mpfr_bits, my_bits;

//     memcpy(&mpfr_bits, &mpfr_result, sizeof(uint64_t));
//     memcpy(&my_bits, &my_result, sizeof(uint64_t));
    
//     return mpfr_bits > my_bits ? mpfr_bits - my_bits : my_bits - mpfr_bits;
// }

uint64_t difference_in_bits(double mpfr_result, double my_result) {
    uint64_t mpfr_bits = *(uint64_t*)&mpfr_result;
    uint64_t my_bits = *(uint64_t*)&my_result;

    return mpfr_bits > my_bits ? mpfr_bits - my_bits : my_bits - mpfr_bits;
}

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
    double R;
    int k = -1;
    int j = (bits >> 52) - 1023;
    
    if (j < -4) R = x;
    else {
        uint64_t mantissa = bits & 0xFFFFFFFFFFFFF;
        k = (mantissa >> (52 + j)) & 0x7;

        int exponent = 1023 + j;
        uint64_t tmp = (uint64_t)exponent << 52;
        double cjk = (*(double*)&tmp) * (1.0 + (k * 0.125));

        // printf("x:%f j:%d k:%d cjk:%f\n\n", x, j, k, cjk);

        R = x - cjk;
    }
    

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

    double cos_R = 0x1p0 + R * 
    (0x1.9a90471eedea2p-101 + R * 
    (-0x1p-1 + R * 
    (-0x1.c35565375f36bp-89 + R * 
    (0x1.5555555555555p-5 + R * 
    (0x1.ff3d9ead9207dp-79 + R * 
    (-0x1.6c16c16c13c9cp-10 + R * 
    (-0x1.a18e77efe123p-70 + R * 
    (0x1.a01a008ec3853p-16 + R * 
    (0x1.be000f0503974p-63 + R * 
    (-0x1.27c32619713b7p-22))))))))));

    double result;
    if (k == -1) {
        result = sin_R;
    }
    else {
        result = table_sin_Cjk[(-1)*j - 1][k - 1] * cos_R + 
                        table_cos_Cjk[(-1)*j - 1][k - 1] * sin_R;
    }
   
    return result;
}

int main() {
    mpfr_t angle_mprf, res_mpfr;
    double PI_4 = 0x1.921fb54442d1846ap-1;
    double min_val = 0x1p-10;
    uint64_t UINT_PI_4 = *(uint64_t*)&PI_4;
    uint64_t UINT_MIN_VAL = *(uint64_t*)&min_val;
    double my_angle, my_res;
    
    init_tables();

    mpfr_init2(angle_mprf, 256);
    mpfr_init2(res_mpfr, 256);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(UINT_MIN_VAL, UINT_PI_4);

    uint64_t max_diff = 0;
    while (true) {
        uint64_t rand_val = dist(gen);

        my_angle = *(double*)&rand_val;
        my_res = sin_minimax(my_angle);

        mpfr_set_d(angle_mprf, my_angle, MPFR_RNDN);
        mpfr_sin(res_mpfr, angle_mprf, MPFR_RNDN);

        printf("VALUE: %lu => %f -> %f\n", rand_val, my_angle, my_res);
        printf("MPFR: sin(rand_x) = ");
        mpfr_printf("%.25Rf\n", res_mpfr);
        printf("MY_FUNC: sin(rand_x) = ");
        printf("%.25f\n", my_res);
        uint64_t diff_res = difference_in_bits(mpfr_get_d(res_mpfr, MPFR_RNDN), my_res);

        if (diff_res > max_diff) max_diff = diff_res;
        
        printf("CURR DIFF: %lu\n", diff_res);
        printf("MAX DIFF: %lu\n", max_diff);
    }

    mpfr_clear(angle_mprf);
    mpfr_clear(res_mpfr);

    return 0;
}