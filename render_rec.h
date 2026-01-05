#ifndef RENDER_REC_H
#define RENDER_REC_H

#include "SDL_rect.h"
#include "screen.h"

inline SDL_Rect square(screen square, int &window_width, int &window_height,
                       int &rec_size) {
    screen p = project(square.x, square.y, square.z);
    screen d = display(p.x, p.y, window_width, window_height);

    return {offset(d.x, rec_size), offset(d.y, rec_size), rec_size, rec_size};
}

#endif // !RENDER_REC_H
