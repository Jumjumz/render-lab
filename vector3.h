#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class vector3 {
  public:
    double e[3];

    vector3() : e{e[0], e[1]} {};

    vector3(double x, double y) : e{x, y} {};

    vector3(double x, double y, double z) : e{x, y, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];
};

inline vector3 rotate(vector3 s, float &angle) {
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    return {s.x * cos - s.z * sin, s.y, s.x * sin + s.z * cos};
}

inline vector3 translate_z(vector3 t, float &dz) {
    return {t.x, t.y, t.z + dz};
}

inline vector3 project(double x, double y, double z) { return {x / z, y / z}; }

inline vector3 display(double x, double y, float &aspect_ratio,
                       int &screen_width, int &screen_height) {
    // -1.. 1 => 0..2 => 0..1 <- by deviding to 2
    return {((x / aspect_ratio) + 1) / 2 * screen_width,
            (1 - (y + 1) / 2) *
                screen_height}; // 1- as y axis is flipped
                                // this can also be used -> return {x *
                                // (screen_width / aspect_ratio) / 2
                                // + double(screen_width) / 2,
                                //             y * screen_height / 2 +
                                //                             double(screen_height)
                                //                             / 2}; // 1- as y
                                //                             axis is flipped
}

inline int offset(double axis, int &size) { return axis - double(size) / 2; }

#endif // !VECTOR3_H
