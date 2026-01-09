#include "objects.h"
#include "sdl_render.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    sdl_render render;

    render.aspect_ratio = 16.0 / 9.0;
    render.window_width = 1440;

    cube cb;
    cb.sides = 0.5;

    cb.get_vertices();
    render.positions = cb.vertices();

    cb.get_edges();
    render.edges = cb.edges();

    // execute functions
    render.run();

    render.destroy();

    return 0;
}
