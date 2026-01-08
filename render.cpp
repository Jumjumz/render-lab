#include "sdl_render.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

int main(int argc, char *argv[]) {
    sdl_render render;

    render.aspect_ratio = 16.0 / 9.0;
    render.window_width = 1440;
    render.window_height = int(render.window_width / render.aspect_ratio);

    render.positions = {{0.25, 0.25, 0.25},    {-0.25, 0.25, 0.25},
                        {-0.25, -0.25, 0.25},  {0.25, -0.25, 0.25},
                        {0.25, 0.25, -0.25},   {-0.25, 0.25, -0.25},
                        {-0.25, -0.25, -0.25}, {0.25, -0.25, -0.25}};

    render.faces = {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    // execute functions
    render.init();

    render.run();

    render.quit();

    return 0;
}
