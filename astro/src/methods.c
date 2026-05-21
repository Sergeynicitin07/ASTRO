#include "methods.h"
#include "math.h"
#include "struct.h"
#include <stdio.h>

// уравнение из геодезической астрономии (построение эфемерид для способа Цингера)
double function (double s0, data *s) {
    double phi = s->phi;
    double declination_w = s->declination_w;
    double declination_e = s->declination_e;
    double right_ascension_e = s->right_ascension_e;
    double right_ascension_w = s->right_ascension_w;
    return sin(phi) * (sin(declination_w) - sin(declination_e))
           + cos(phi) * (cos(declination_w) * cos(s0 - right_ascension_w)
                         - cos(declination_e) * cos(s0 - right_ascension_e));
}


double Bisection_method(double (*function)(double, data*),
                        double a, double b, data *s, int i) {
    double c;
    if (i == 1)
        printf("Iteration of Bisection_method\n");
    // для подбора стартового шага в методе сэра Ньютона
    if (i == 0) {
        while (fabs(b - a) > 5e-2) {
            c = (a + b) / 2.0; // находим середину


            if (function(a, s) * function(c, s) > 0.0) {
                a = c;
            } else {
                b = c;
            }
        }

    }
    if (i == 1) {
        while (fabs(b - a) > 1e-15) {
            c = (a + b) / 2.0; // находим середину

            printf("%.15le\n", c);

            if (function(a, s) * function(c, s) > 0.0) {
                a = c;
            } else {
                b = c;
            }
        }
    }

    return (a + b) / 2.0;
}


double d_function (double s0, data *s) {
    double phi = s->phi;
    double declination_w = s->declination_w;
    double declination_e = s->declination_e;
    double right_ascension_e = s->right_ascension_e;
    double right_ascension_w = s->right_ascension_w;
    return cos(phi) * (cos(declination_e)
                       * sin(s0 - right_ascension_e)
                       - cos(declination_w)
                         * sin(s0 - right_ascension_w));
}


double get_s0(data *s) {
    // середина между звездами
    double s_approx = normalize_angle((s->right_ascension_e + s->right_ascension_w) / 2.0);


    // создаем узкое и безопасное окно в +-0.5 радиана (около 2 часов)
    double a = s_approx - 0.1;
    double b = s_approx + 0.1;

    // проверяем условие Больцано-Коши
    if (function(a, s) * function(b, s) >= 0.0) {
        a = s_approx - 0.5;
        b = s_approx + 0.5;

        if (function(a, s) * function(b, s) >= 0.0) {
            return 0;
        }
    }


    double s0 = Bisection_method(function, a, b, s, 0);
    return s0;
}


void get_a_b(data *s, double *a, double *b) {
    // середина между звездами
    double s_approx = normalize_angle((s->right_ascension_e + s->right_ascension_w) / 2.0);


    // создаем узкое и безопасное окно в +-0.5 радиана (около 2 часов)
    *a = s_approx - 0.1;
    *b = s_approx + 0.1;

    // проверяем условие Больцано-Коши
    if (function(*a, s) * function(*b, s) >= 0.0) {
        *a = s_approx - 0.5;
        *b = s_approx + 0.5;

        if (function(*a, s) * function(*b, s) >= 0.0) {
            *a = 0;
            *b = 0;
        }
    }


}


double Sir_Isaac_Newton_method (double (*function)(double, data*),
                                double (*d_function)(double, data*),
                                double s0, data *s) {

    double d_function1 = d_function(s0, s);
    if (d_function1 == 0) return s0;
    printf("Iteration of Newton_method\n");
    double function1 = function(s0, s);
    double x = s0 - function1/d_function1;
    printf("%.15le\n", x);

    while (fabs(x - s0) > 1e-15) {
        d_function1 = d_function(x, s);
        if (d_function1 == 0) return x;

        function1 = function(x, s);
        s0 = x;
        x -= function1/d_function1;
        printf("%.15le\n", x);
    }
    return x;

}


void time_write (double s0) {
    // если корень отрицательный, сделае м его положительным, используя тригонометрический период 2PI
    s0 = normalize_angle(s0);
    double total_hours = s0 * (12.0 / M_PI);

    int hours = (int)total_hours;

    double total_minutes = (total_hours - hours) * 60.0;
    int minutes = (int)total_minutes;

    double seconds = (total_minutes - minutes) * 60.0;
    hours = hours % 24;
    minutes = minutes % 60;

    printf("%02d : hour\t%02d : min\t%05.2f : sec\n", hours, minutes, seconds);

}

