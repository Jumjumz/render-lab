#ifndef SDL_H
#define SDL_H

#include <SDL.h>
#include <SDL_events.h>
#include <sys/types.h>

class sdl {
  public:
    uint window_width = 720;
    float aspect_ratio = 1.0;

    void run() {
        init();

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
            }
        }
    }

    void destroy() const {
        SDL_DestroyWindow(window);

        SDL_Quit();
    }

  private:
    uint window_height;
    bool running = true;

    SDL_Window *window;
    SDL_Event event;

    void init() {
        window_height = uint(window_width / aspect_ratio);
        window_height =
            (window_height < 1)
                ? 1
                : window_height; // make sure height is not 0 or negative

        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow("Wireframe Renderer", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, window_width,
                                  window_height, SDL_WINDOW_SHOWN);
    }
};

#endif // !SDL_H
