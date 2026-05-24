#include "methods.h"
#include "math.h"

#include "struct.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

double get_time() {
#ifdef _WIN32
    static LARGE_INTEGER freq;
    static int initialized = 0;
    LARGE_INTEGER now;

    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    QueryPerformanceCounter(&now);
    return (double) now.QuadPart / freq.QuadPart;

#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec / 1e9;
#endif
}

// уравнение из геодезической астрономии (построение эфемерид для способа Цингера)
double function(double s0, data *s) {
    double phi = s->phi;
    double dw = s->declination_w, de = s->declination_e;
    double aw = s->right_ascension_w, ae = s->right_ascension_e;
    double sin_phi = sin(phi), cos_phi = cos(phi);
    double cos_hw = sin_phi * sin(dw) + cos_phi * cos(dw) * cos(s0 - aw);
    double cos_he = sin_phi * sin(de) + cos_phi * cos(de) * cos(s0 - ae);
    return cos_hw - cos_he;
}


double d_function(double s0, data *s) {
    double phi = s->phi;
    double dw = s->declination_w, de = s->declination_e;
    double aw = s->right_ascension_w, ae = s->right_ascension_e;
    double cos_phi = cos(phi);
    // производная от cos_hw по s0: -cos_phi * cos(dw) * sin(s0 - aw)
    // производная от cos_he: -cos_phi * cos(de) * sin(s0 - ae)
    return cos_phi * (cos(de) * sin(s0 - ae) - cos(dw) * sin(s0 - aw));
}






double Bisection_method(double (*function)(double, data*), double a, double b, data *s, int i) {
    double start = get_time();
    double time_limit = 2.0;
    double c;
    printf("Iteration of Bisection_method\n");
    while (1) {
        c = a + ((b - a) / 2.0);
        double elapsed = get_time() - start;
        if (elapsed > time_limit) {
            printf("Shock\tand\tawe\n");
            break;
        }

        printf("%.15le\n", c);

        if (c == a || c == b) {
            break;
        }

        if (signbit(function(a, s)) ==  signbit(function(c, s))) {
            a = c;
        } else {
            b = c;
        }
    }
    return c;
}



double gets0(data *s, double *a, double *b, double *s0) {
    int N = 1000;
    double data = (s->right_ascension_e + s->right_ascension_w) / 2;
    *s0 = (s->right_ascension_e + s->right_ascension_w) / 2;
    *a = (*s0 - 1);
    *b = (*s0 + 1);
    double j1 = *s0;
    double j2 = *s0;
    double a1 = *s0 - (M_PI / 2);
    double a2 = *s0 - (M_PI / 2);
    double b1 = *s0 + (M_PI / 2);
    double b2 = *s0 + (M_PI / 2);

    int k = 0;
    double step = 2.0 * M_PI / N;

    for (int i = 0; i < N; i++) {
        double x1 = i * step;
        double x2 = (i + 1) * step;

        // eсли знаки разные - корень существует по теорема Больцано-Коши
        if (function(x1, s) * function(x2, s) < 0.0) {


            if (k == 0) {
                j1 = (x1 + x2) / 2.0;
                k ++;
                a1 = x1;
                b1 = x2;

            } else {
                j2 = (x1 + x2) / 2.0;
                a2 = x1;
                b2 = x2;
            }

        }
    }

    if (j1 < data + M_PI / 2 && j1 > data - M_PI / 2) {
        *a = a1;
        *b = b1;
        return *s0 = j1;
    } else {
        *a = a2;
        *b = b2;
        return *s0 = j2;
    }
}

// по пути rtsafe - страница 460. Root Finding and Nonlinear Sets of Equations.
double Sir_Isaac_Newton_method(double (*function)(double, data*),
                               double (*d_function)(double, data*),
                               double s0, data *s,  double *a, double *b, double x2)

{

    double x = s0;
    double f_a = function(*a, s); // значение функции на левом краю отрезка, откуда родом стартовая точка s0

    printf("Iteration of Newton_method\n");
    double start = get_time();
    double time_limit = 1.0;

    while (1) {
        double elapsed = get_time() - start;
        if (elapsed > time_limit) {
            printf("Shock\tand\tawe\n");
            break;
        }

        double f = function(x, s);
        double df = d_function(x, s);

        if (signbit(f) == signbit(f_a)) {
            *a = x;
            f_a = f;
        } else {
            *b = x;
        }


        double x_next;
        if (fabs(df) == 0.0) {
            x_next = *a + (*b - *a) / 2.0;
        } else {
            double p = -f / df;
            x_next = x + p;

            double min_bound = (*a < *b) ? *a : *b;
            double max_bound = (*a > *b) ? *a : *b;

            if (x_next <= min_bound || x_next >= max_bound) {
                x_next = *a + (*b - *a) / 2.0;
            }
        }

        if (x == x_next) {
            break;
        }

        x = x_next;
        printf("%.15le\n", x);

    }

    return x;
}


void time1_write (double s0) {
    // если корень отрицательный, сделае м его положительным, используя тригонометрический период 2PI
    s0 = normalize_angle(s0);
    double total_hours = s0 * (12.0 / M_PI);

    int hours = (int)total_hours;

    double total_minutes = (total_hours - hours) * 60.0;
    int minutes = (int)total_minutes;

    double seconds = (total_minutes - minutes) * 60.0;

    hours = hours % 24;
    minutes = minutes % 60;
    if (fabs(seconds - 60.0) > 0 && fabs(seconds - 60.0) < 1e-5) {
        seconds = 0.0;
        minutes ++;
    }
    if (minutes >= 60) {
        minutes -= 60;
        hours ++;
    }
    if (hours >= 24) hours = 0;

    printf("%02d : hour\t%02d : min\t%05.2f : sec\n", hours, minutes, seconds);

}
