#include "methods.h"
#include "math.h"
#include <float.h>

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

    *s0 = (s->right_ascension_e + s->right_ascension_w) / 2;
    *a = (*s0 - 1);
    *b = (*s0 + 1);
    double step = 2.0 * M_PI / N;

    for (int i = 0; i < N; i++) {
        double x1 = i * step;
        double x2 = (i + 1) * step;

        // eсли знаки разные - корень существует по теорема Больцано-Коши
        if (function(x1, s) * function(x2, s) < 0.0) {
            
            *a = x1;
            *b = x2;
            *s0 = (x1 + x2) / 2.0;
            if (*s0 < data + 0.5 && *s0 > data - 0.5)
                return *s0;
        }
    }
    return *s0;
}


// TENSOR METHOD (11.33)
// страница 283 и тд
// Nocedal.pdf
double Sir_Isaac_Newton_method(double (*function)(double, data*),
                            double (*d_function)(double, data*),
                            double s0, data *s,  double *a, double *b, double x2)
{

    double x = s0;
    double x_last;
    double r;

    printf("Iteration of Newton_method\n");
    double start = get_time();
    double time_limit = 1.0;

    printf("%.15le\n", x);

    double r_k = function(x, s);
    double J_k = d_function(x, s);

    if (fabs(J_k) < 1e-12) return x;

    double p = -r_k / J_k; // обычные вычисления по базовому алгоритму Ньютона
    double x_next = x + p;


    while (1) {
        double elapsed = get_time() - start;
        if (elapsed > time_limit) {
            printf("Shock\tand\tawe\n");
            break;
        }

        printf("%.15le\n", x_next);


        if (x_next == x) {
            return x_next;
        }


        x_last = x;
        r = r_k;
        x = x_next;

        r_k = function(x, s);
        J_k = d_function(x, s);

        if (fabs(J_k) < 1e-14) break;

        // (аппроксимация 2-й производной через одномерный тензер)
        double dx = x_last - x;
        double T_k = 0.0;

        if (fabs(dx) > 1e-12) {
            T_k = 2.0 * (r - r_k - J_k * dx) / (dx * dx);
        }

        // нужен корень 0.5*T_k*p^2 + J_k*p + r_k = 0
        // считаем дискриминант
        double dis = J_k * J_k - 2.0 * T_k * r_k;

        if (dis >= 0.0 && fabs(T_k) > 1e-13) {
            double sign_J = (J_k > 0) ? 1.0 : -1.0;
            p = (-2.0 * r_k) / (J_k + sign_J * sqrt(dis));
        } else {
            p = -r_k / J_k;
        }
        if (fabs(p) <= fabs(x) * DBL_EPSILON) {
            return x_next;
        }

        if (x == x2) return x;
        

    }

    return x_next;
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

    printf("%02d : hour\t%02d : min\t%05.2f : sec\n", hours, minutes, seconds);

}

