
#include "util.h"

float map(float x, float a, float b, float c, float d) {
    float t = (x - a) / (b - a);
    return (1 - t) * c + t * d;
}
