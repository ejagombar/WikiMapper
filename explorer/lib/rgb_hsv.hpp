#include <math.h>

typedef struct RGBColor {
    char r;
    char g;
    char b;
} RGBColor;

RGBColor hsv2rgb(const float h, const float s, const float v);
