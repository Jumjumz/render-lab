#include "cube.h"
#include "sdl_render.h"
#include "spheres.h"

#include <SDL2/SDL.h>
#include <memory>

int main(int argc, char *argv[]) {
    sdl_render renderer;

    renderer.aspect_ratio = 16.0 / 9.0;
    renderer.window_width = 1440;

    // execute functions
    renderer.run(std::make_shared<spheres>(0.5));

    renderer.destroy();

    return 0;
}
