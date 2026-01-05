#ifndef FRAME_H
#define FRAME_H

#include "SDL_rect.h"
#include "screen.h"

inline SDL_Rect frame(double delta_time, int &window_width, int &window_height,
                      int &rec_size) {
    int dz = 0;
    dz += 1 * delta_time;

    screen s(0.5, 0.5, 1 + dz);
    screen p = project(s.x, s.y, s.z);
    screen d = display(p.x, p.y, window_width, window_height);

    return {offset(d.x, rec_size), offset(d.y, rec_size), rec_size, rec_size};
}

#endif // !FRAME_H
