#include "methods.h"
#include "math.h"
#include "struct.h"
#include <stdio.h>
#include <float.h>
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
    double sin_hw = sin_phi * sin(dw) + cos_phi * cos(dw) * cos(s0 - aw);
    double sin_he = sin_phi * sin(de) + cos_phi * cos(de) * cos(s0 - ae);
    return sin_hw - sin_he;
}


double d_function(double s0, data *s) {
    double phi = s->phi;
    double dw = s->declination_w, de = s->declination_e;
    double aw = s->right_ascension_w, ae = s->right_ascension_e;
    double cos_phi = cos(phi);
    // производная от sin_hw по s0: -cos_phi * cos(dw) * sin(s0 - aw)
    // производная от sin_he: -cos_phi * cos(de) * sin(s0 - ae)
    return cos_phi * (cos(de) * sin(s0 - ae) - cos(dw) * sin(s0 - aw));
}



double Bisection_method(double (*function)(double, data*), double a, double b, data *s, int i) {
    double start = get_time();
    double time_limit = 17.0;
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

        if (function(a, s) * function(c, s) > 0.0) {
            a = c;
        } else {
            b = c;
        }
    }
    return c;
}



double gets0(data *s, double *a, double *b, double *s0) {
    int N = 1000;
    double step = 2.0 * M_PI / N;

    for (int i = 0; i < N; i++) {
        double x1 = i * step;
        double x2 = (i + 1) * step;

        // eсли знаки разные - корень существует по теорема Больцано-Коши
        if (function(x1, s) * function(x2, s) < 0.0) {
            *a = x1;
            *b = x2;
            *s0 = (x1 + x2) / 2.0;
            return *s0;
        }
    }
    return 0.0;
}



double Sir_Isaac_Newton_method(double (*function)(double, data*), double (*d_function)(double, data*), double s0, data *s) {
    double x = s0;
    printf("Iteration of Newton_method\n");
    double start = get_time();
    double time_limit = 15.0;

    while (1) {
        double elapsed = get_time() - start;
        if (elapsed > time_limit) {
            printf("Shock\tand\tawe\n");
            break;
        }


        if (d_function(x, s) == 0.0) return x;

        double x_next = x - (function(x, s) / d_function(x, s));

        printf("%.15le\n", x_next);



        if (fabs(x_next - x) <= DBL_EPSILON * fabs(x_next) || x_next == x) {
            return x_next;
        }
        if (fabs(function(x_next, s)) >= fabs(function(x, s)) && fabs(x_next - x) < 1e-15) {
            return x;
        }

        x = x_next;
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

