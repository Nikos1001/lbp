
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fstream>
#include "../lib/glm/glm/glm.hpp"
#include "../lib/glm/glm/gtc/matrix_transform.hpp"
#include "../lib/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

#define str(...) #__VA_ARGS__

float map(float x, float a, float b, float c, float d);

#endif