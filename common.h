#define ALLOW_REDEFINE extern __attribute__((weak))

#include "math.h"

inline float ab2c(float a, float b) {
    return sqrt(a * a + b * b);
}