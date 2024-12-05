#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mpfr.h>

int main() {
    for (int j = -1; j >= -4; j--) {
        for (int k = 1; k <= 7; k++) {
            int exponent = 1023 + j;
            uint64_t tmp = (uint64_t)exponent << 52;
            double temp_x = (*(double*)&tmp) * (1.0 + (k * 0.125));


            uint64_t bits = *(uint64_t*)&temp_x;
            uint64_t sign_mask = 0x7FFFFFFFFFFFFFFF;
            bits &= sign_mask;
            int res_k;
            int res_j = (bits >> 52) - 1023;
            
            uint64_t mantissa = bits & 0xFFFFFFFFFFFFF;
            res_k = (mantissa >> 49) & 0x7;

            exponent = 1023 + res_j;
            tmp = (uint64_t)exponent << 52;
            double cjk = (*(double*)&tmp) * (1.0 + (res_k * 0.125));

            printf("%d; %f => cjk: %f; x: %f\n", cjk == temp_x, temp_x - cjk, cjk, temp_x);
        }
    }

    return 0;
}