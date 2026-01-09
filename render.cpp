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

    render.edges = {{0, 1}, {3, 2}, {4, 5}, {7, 6}, {0, 4}, {1, 5},
                    {2, 6}, {3, 7}, {0, 2}, {4, 6}, {1, 3}, {5, 7}}; // edges

    // execute functions
    render.run();

    render.destroy();

    return 0;
}
