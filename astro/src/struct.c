#include "struct.h"
#include <math.h>


const double d_2pi = {
        6.283185307179586232e+00
};


// если передали не радианы, а градусы
void angles (data *s) {
    s->phi = s->phi * M_PI / 180.0;
    s->right_ascension_w = s->right_ascension_w * M_PI / 180.0;
    s->right_ascension_e = s->right_ascension_e * M_PI / 180.0;
    s->declination_e = s->declination_e * M_PI / 180.0;
    s->declination_w = s->declination_w * M_PI / 180.0;
}


double normalize_angle (double angle) {
    angle = fmod (angle, 2.0 * M_PI);
    if (angle < 0) {
        angle += 2.0 * M_PI;
    }
    return angle;
}