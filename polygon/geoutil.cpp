
#include "geoutil.h"

float triArea(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    return fabs(
        a.x * (b.y - c.y) +
        b.x * (c.y - a.y) +
        c.x * (a.y - b.y)
    );
}

bool ptInsideTri(glm::vec2 pt, glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    float A  = triArea(a, b, c);
    float A1 = triArea(a, b, pt);
    float A2 = triArea(b, c, pt);
    float A3 = triArea(a, c, pt);
    return fabs(A - (A1 + A2 + A3)) < 0.00001f;
}
