#include "objects.h"
#include "sdl_render.h"

#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

int main(int argc, char *argv[]) {
    sdl_render render;

    render.aspect_ratio = 16.0 / 9.0;
    render.window_width = 1440;

    cube cb;
    cb.sides = 0.5;
    cb.get_vertices();

    render.positions = cb.vertices();

    render.faces = {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4},
                    {1, 5},       {2, 6},       {3, 7}}; // vertices

    // execute functions
    render.run();

    render.destroy();

    return 0;
}
