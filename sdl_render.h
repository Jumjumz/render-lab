#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "vec.h"
#include <vector>

class sdl_render {
  public:
    int window_width;
    int window_height;
    std::vector<vec> positions;
    std::vector<std::vector<vec>> faces;

  private:
    SDL_Window *window = SDL_CreateWindow(
        "Wireframe Render", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
};

#endif // !SDL_RENDER_H
