#ifndef UNTITLED37_STRUCT_H
#define UNTITLED37_STRUCT_H


typedef struct S{
    double phi;
    // склонение звезды
    double declination_w;
    double declination_e;
    // прямое восхождение от точки равноденствия для восточной и западной звезды
    double right_ascension_e;
    double right_ascension_w;

}data;


extern const double d_2pi;


void angles (data *s);


double normalize_angle (double angle);


#endif //UNTITLED37_STRUCT_H
