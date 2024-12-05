#include <string.h>
#include <stdio.h>
#include <random>
#include <mpfr.h>
#include <cmath>
#include <cstdint>

uint64_t table_sin_Cjk[32] = {0x3fe0362939c69955, 0x3fe1e7343236574c, 0x3fe386597456282b, 0x3fe511f9fd7b351c, 
0x3fe6888a4e134b2f, 0x3fe7e893f5037959, 0x3fe930b705f9f85a, 0x3fea5fab793d29c8, 
0x3fd1c37d64c6b876, 0x3fd3ad129769d3d8, 0x3fd591bc9fa2f597, 0x3fd7710255764214, 
0x3fd94a6be9f546c5, 0x3fdb1d8305321617, 0x3fdce9d2e3d4a51f, 0x3fdeaee8744b05f0, 
0x3fc3eb312c5d66cb, 0x3fc5e44fcfa126f3, 0x3fc7dc102fbaf2b5, 0x3fc9d252d0cec312, 
0x3fcbc6f84edc6199, 0x3fcdb9e15fb5a5d0, 0x3fcfaaeed4f31577, 0x3fd0cd00cef36436, 
0x3fb7f701032550e4, 0x3fb9f4902d55d1f9, 0x3fbbf1b78568391d, 0x3fbdee6f16c1cce6, 
0x3fbfeaaeee86ee36, 0x3fc0f3378ddd71d1, 0x3fc1f0d3d7afceaf, 0x3fc2ee285e4ab88f};

uint64_t table_cos_Cjk[32] = {0x3feb96eeef58840e, 0x3fea85ed4373e02d, 0x3fe95a67e00cb1fd, 0x3fe8158a31916d5d, 
0x3fe6b898fa9efb5d, 0x3fe544f10f592ca5, 0x3fe3bc05f8b3a656, 0x3fe21f608107e37a, 
0x3feebe214f76efa8, 0x3fee733ea0193d40, 0x3fee20bf49acd6c1, 0x3fedc6b7eb995912, 
0x3fed653f073e4040, 0x3fecfc6cfa52ad9f, 0x3fec8c5bf8ce1a84, 0x3fec1528065b7d50, 
0x3fef9c340a7cc428, 0x3fef874c2e1eecf6, 0x3fef706bdf9ece1c, 0x3fef57948cff6797, 
0x3fef3cc7c3b3d16e, 0x3fef20073086649f, 0x3fef01549f7deea1, 0x3feee0b1fbc0f11c, 
0x3fefdc06bf7e6b9b, 0x3fefd5c94b43e000, 0x3fefcf0c800e99b1, 0x3fefc7d078d1bc88, 
0x3fefc015527d5bd3, 0x3fefb7db2bfe0695, 0x3fefaf22263c4bd3, 0x3fefa5ea641c36f2};

uint64_t difference_in_bits(double mpfr_result, double my_result) {
    uint64_t mpfr_bits = *(uint64_t*)&mpfr_result;
    uint64_t my_bits = *(uint64_t*)&my_result;

    return mpfr_bits > my_bits ? mpfr_bits - my_bits : my_bits - mpfr_bits;
}

double sin_minimax(double x) {
    uint64_t bits = *(uint64_t*)&x;
    uint64_t sign_mask = 0x7FFFFFFFFFFFFFFF;
    uint64_t sign = 0x8000000000000000 & bits;
    bits &= sign_mask;
    double R;
    int k = -1;
    int j = (bits >> 52) - 1023;
    
    if (j < -4) R = x;
    else {
        uint64_t mantissa = bits & 0xFFFFFFFFFFFFF;
        k = (mantissa >> 49) & 0x7;

        int exponent = 1023 + j;
        uint64_t tmp = (uint64_t)exponent << 52;
        double cjk = (*(double*)&tmp) * (1.0 + (k * 0.125));

        // printf("x:%f j:%d k:%d cjk:%f\n\n", x, j, k, cjk);

        R = x - (cjk + 0x1p-5);
    }
    
    double double_R = R*R;

    double sin_R = R * (0x1p0 + double_R * 
    (-0x1.5555555555544p-3 + double_R * 
    (0x1.1111110f7104p-7 + double_R * 
    (-0x1.a01738588943ap-13))));


    double cos_R = 0x1p0 + double_R * 
    (-0x1.fffffffffffb2p-2 + double_R * 
    (0x1.55555551ad371p-5 + double_R * 
    (-0x1.6c139efb45fc6p-10)));

    double result;
    if (k == -1) {
        result = sin_R;
    }
    else {
        int col_ind = k;
        int row_ind = (-1)*j - 1;
        uint64_t* cjk_sin = (table_sin_Cjk + row_ind * 8 + k);
        uint64_t* cjk_cos = (table_cos_Cjk + row_ind * 8 + k);
        double d_cjk_sin = *(double*)cjk_sin;
        double d_cjk_cos = *(double*)cjk_cos;
        result = d_cjk_sin * cos_R + d_cjk_cos * sin_R;
    }
    if (sign == 0)
        return result;
    else
        return -1 * result;
}

int main() {
    mpfr_t angle_mprf, res_mpfr;
    double PI_4 = 0x1.921fb54442d1846ap-1;
    double min_val = 0x1p-10;
    uint64_t UINT_PI_4 = *(uint64_t*)&PI_4;
    uint64_t UINT_MIN_VAL = *(uint64_t*)&min_val;
    double my_angle, my_res;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(UINT_MIN_VAL, UINT_PI_4);

    mpfr_init2(angle_mprf, 256);
    mpfr_init2(res_mpfr, 256);

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