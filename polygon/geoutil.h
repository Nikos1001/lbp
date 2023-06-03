
#ifndef GEOUTIL_H
#define GEOUTIL_H

#include "../util.h"

float triArea(glm::vec2 a, glm::vec2 b, glm::vec2 c);
bool ptInsideTri(glm::vec2 pt, glm::vec2 a, glm::vec2 b, glm::vec2 c);

#endif
