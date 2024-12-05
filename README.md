# Состояние задачи

## Функция синус

### Исправленные ошибки:
1. Была найдена причина ошибки и исправлена ошибка с периодически возникающей большой величиной разницы между моим синусом и mpfr синусом.
2. Таблицы синусов и косинусов теперь хранят uint64_t значения синусов и косинусов Cjk.
3. У sollya получил другие разложения только с необходимыми степенями полинома.

### Текущий результат:
Однако, при запуске бесконечного цикла проверки с mpfr разница составляет от 0 до 4 бит. То есть максимальное отклонение 4 бита.
Причину выяснить пока не удалось.

### Доп. Информация:
1. Код по выводу таблиц синусов и косинусов Cjk:
   ```c++
   #include <iostream>
    #include <cstdint>
    #include <mpfr.h>

    int main() {
        mpfr_t cjk, sin_cjk, cos_cjk, mpfr_1_32;
        
        mpfr_init2(cjk, 256);
        mpfr_init2(sin_cjk, 256);
        mpfr_init2(cos_cjk, 256);
        mpfr_init2(mpfr_1_32, 256);

        double d_1_32 = 0x1p-5;
        mpfr_set_d(mpfr_1_32, d_1_32, MPFR_RNDN); 
        std::cout << "{";
        for (int j = -1; j >= -4; j--) {
            for (int k = 0; k <= 7; k++) {
                int exponent = 1023 + j;
                uint64_t tmp = (uint64_t)exponent << 52;
                double cjk_value = (*(double*)&tmp) * (1.0 + (k * 0.125));

                mpfr_set_d(cjk, cjk_value, MPFR_RNDN);
                mpfr_add(cjk, cjk, mpfr_1_32, MPFR_RNDN);
                //mpfr_sin(sin_cjk, cjk, MPFR_RNDN);
                mpfr_cos(cos_cjk, cjk, MPFR_RNDN);
                //double res = mpfr_get_d(sin_cjk, MPFR_RNDN);
                double res = mpfr_get_d(cos_cjk, MPFR_RNDN);
                uint64_t* ptr = reinterpret_cast<uint64_t*>(&res);

                std::cout << std::hex << "0x" << *ptr << ", ";
            }
        }

        return 0;
    }
    ```
2. Используемые мной команды для sollya: sollya_commands.txt (в прикрепленных файлах):
    #### preparing:
    guessdegree(cos(x),[-1b-5;1b-5],1b-55);
    guessdegree(sin(x),[-1b-5;1b-5],1b-55);

    #### Cos commands:
    monomials = [||];
    for i from 0 to 3 do { monomials = monomials :. x^(i*2); };
    q = fpminimax(cos(x), monomials, [|D...|], [-1b-5;1b-5]);
    display = hexadecimal;
    q;

    #### Sin commands:
    monomials = [||];
    for i from 0 to 3 do { monomials = monomials :. x^(i*2 + 1); };
    q = fpminimax(sin(x), monomials, [|D...|], [0;1b-5]);
    display = hexadecimal;
    q;