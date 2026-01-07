#ifndef RENDER_POINT_H
#define RENDER_POINT_H

#include "vector3.h"

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
