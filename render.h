#ifndef RENDER_POINT_H
#define RENDER_POINT_H

#include "vector3.h"

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
            (1 - (y + 1) / 2) * screen_height}; // 1- as y axis is flipped
    // this can also be used like this:
    // return {x * (screen_width / aspect_ratio) / 2 + double(screen_width) / 2,
    // y * screen_height / 2 + double(screen_height) / 2};
}

inline int offset(double axis, int &size) { return axis - double(size) / 2; }

inline vector3 point(vector3 point, float &aspect_ratio, int &window_width,
                     int &window_height) {
    vector3 p = project(point.x, point.y, point.z);
    vector3 d = display(p.x, p.y, aspect_ratio, window_width, window_height);

    return {d.x, d.y};
}

inline vector3 vertex(vector3 vtx, float &aspect_ratio, int &window_width,
                      int &window_height) {
    vector3 p = project(vtx.x, vtx.y, vtx.z);
    vector3 d = display(p.x, p.y, aspect_ratio, window_width, window_height);

    return {d.x, d.y, d.z};
}

#endif // !RENDER_POINT_H
