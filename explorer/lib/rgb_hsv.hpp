#include <math.h>

typedef struct RGBColor {
    float r;
    float g;
    float b;
} RGBColor;

RGBColor hsv2rgb(float h, float s, float v);
