#ifndef RGB_HSV_H
#define RGB_HSV_H

#include "./rgb_hsv.hpp"

RGBColor hsv2rgb(const float h, const float s, const float v) {
    float r, g, b = 0.0f;

    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
    case 0:
        r = v, g = t, b = p;
        break;
    case 1:
        r = q, g = v, b = p;
        break;
    case 2:
        r = p, g = v, b = t;
        break;
    case 3:
        r = p, g = q, b = v;
        break;
    case 4:
        r = t, g = p, b = v;
        break;
    case 5:
        r = v, g = p, b = q;
        break;
    }

    RGBColor color;
    color.r = static_cast<char>(r * 255);
    color.g = static_cast<char>(g * 255);
    color.b = static_cast<char>(b * 255);

    return color;
}

#endif
