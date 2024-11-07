#include <stdio.h>
#include <mpfr.h>

double sin_minimax(double x) {
    double sin_R = 0x1.c5f0261f1868cp-95 + x * (0x1p0 + x * (-0x1.f41bcf0494c2ap-81 + x * (-0x1.5555555555555p-3 + x * (0x1.f93211fd72046p-70 + x * (0x1.111111110e667p-7 + x * (-0x1.59e56126ed6f9p-60 + x * (-0x1.a019ffd908d95p-13 + x * (0x1.7e72a473c5d21p-52 + x * (0x1.71621f6a299f4p-19 + x * (-0x1.268826342f202p-45 + x * 0x1.62f40c5e99247p-22))))))))));

    return sin_R;
}

int main() {
    mpfr_t angle_pi_16, angle_pi_32, sin_res_16, sin_res_32;
    double Pi = 3.14159265358979323846;
    double pi_16 = Pi / 16, pi_32 = Pi / 32;
    double res_pi_16, res_pi_32;
    
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