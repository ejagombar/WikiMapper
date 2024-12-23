#include <math.h>

typedef struct RGBColor {
    char r;
    char g;
    char b;
} RGBColor;

RGBColor hsv2rgb(float h, float s, float v);
