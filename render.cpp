#include "cube.h"
#include "sdl_render.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    sdl_render renderer;

    renderer.aspect_ratio = 16.0 / 9.0;
    renderer.window_width = 1440;

    // execute functions
    renderer.run(std::make_shared<cube>(0.5));

    renderer.destroy();

    return 0;
}
