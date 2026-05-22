#ifndef UNTITLED37_METHODS_H
#define UNTITLED37_METHODS_H
#include "methods.h"
#include "math.h"
#include "struct.h"
#include <stdio.h>

// уравнение из геодезической астрономии (построение эфемерид для способа Цингера)
double function (double s0, data *s);


double Bisection_method(double (*function)(double, data*),
                        double a, double b, data *s, int i);


double d_function (double s0, data *s);


double Sir_Isaac_Newton_method (double (*function)(double, data*),
                                double (*d_function)(double, data*),
                                double s0, data *s);


double gets0(data *s, double *a, double *b, double *s0);


void time_write (double s0);


#endif //UNTITLED37_METHODS_H
