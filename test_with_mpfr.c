#include <stdio.h>
#include <mpfr.h>

double sin_minimax(double x) {
    double sin_R = ((((((-0.00018236 * x + 
                -0.0000273543) * x + 
                0.00835389) * x + 
                -8.15912e-6) * x + 
                -0.166665) * x + 
                -1.81057e-7) * x + 
                1.0) * x + 
                -6.42047e-11;

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