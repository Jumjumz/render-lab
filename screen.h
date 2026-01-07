#ifndef SCREEN_H
#define SCREEN_H

#include "vec.h"

inline vec rotate(vec s, float &angle) {
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    return {s.x * cos - s.z * sin, s.y, s.x * sin + s.z * cos};
}

inline vec translate_z(vec t, float &dz) { return {t.x, t.y, t.z + dz}; }

inline vec project(double x, double y, double z) { return {x / z, y / z}; }

#endif // !SCREEN_H
