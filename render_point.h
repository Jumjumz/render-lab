#ifndef RENDER_POINT_H
#define RENDER_POINT_H

#include "SDL_rect.h"
#include "screen.h"

inline SDL_Point point(screen point, float &aspect_ratio, int &window_width,
                       int &window_height) {
    screen p = project(point.x, point.y, point.z);
    screen d = display(p.x, p.y, aspect_ratio, window_width, window_height);

    return {int(d.x), int(d.y)};
}

inline screen vertex(screen vtx, float &aspect_ratio, int &window_width,
                     int &window_height) {
    screen p = project(vtx.x, vtx.y, vtx.z);
    screen d = display(p.x, p.y, aspect_ratio, window_width, window_height);

    return {d.x, d.y, d.z};
}

#endif // !RENDER_POINT_H
