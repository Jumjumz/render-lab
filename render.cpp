#include "cube.h"
#include "sdl_render.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    sdl_render renderer;

    renderer.aspect_ratio = 16.0 / 9.0;
    renderer.window_width = 1440;
    // renderer.run(std::make_shared<cube>(0.5));

    cube cb;
    cb.sides = 0.5;

    cb.get_vertices();
    renderer.positions = cb.vertices();

    cb.get_edges();
    renderer.edges = cb.edges();

    // execute functions
    renderer.run();

    renderer.destroy();

    return 0;
}
