#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "struct.h"
#include "methods.h"


int main(int argc, char *argv[]) {

    data s;
    // Задача работает для двух звезд на одном альмукантарате в плоскости 1-го вертикала
    // Широта Петербурга
    // данные в градусах
    s.phi = 59.95;



    // Тета Близнецов

    s.declination_e = 34.0;

    s.right_ascension_e = 98.7;


    // Тау Геркулеса

    s.declination_w = 32.2;

    s.right_ascension_w = 265.4;
    // Перевод в радианы

    // s0 = 0;

    if (argc > 1)
        s.phi = atof(argv[1]);
    if (argc > 2)
        s.declination_e = atof(argv[2]);
    if (argc > 3)
        s.declination_w = atof(argv[3]);
    if (argc > 4)
        s.right_ascension_e = atof(argv[4]);
    if (argc > 5)
        s.right_ascension_w = atof(argv[5]);

    angles(&s);
    double s0;
    double a;
    double b;
    s0 = gets0(&s, &a, &b, &s0);
    double x2 = Bisection_method(function, a, b, &s, 1);
    double x1 = Sir_Isaac_Newton_method(function, d_function, s0, &s, &a, &b, x2);
    printf("Result of Newton_method:\n");
    printf("%.15le\n", x1);
    time1_write(x1);
    printf("Result of Bisection_method:\n");
    printf("%.15le\n", x2);
    time1_write(x2);



    return 0;
}
