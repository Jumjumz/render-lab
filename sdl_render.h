#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include "SDL.h"
#include "screen.h"
#include "vec.h"
#include <cmath>
#include <vector>

class sdl_render {
  public:
    int window_width = 100;
    int window_height = 100;
    float aspect_ratio = 1.0;

    std::vector<vec> positions;
    std::vector<std::vector<int>> faces;

    void init() {
        SDL_Init(SDL_INIT_EVERYTHING);

        window = SDL_CreateWindow("Wireframe Render", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, window_width,
                                  window_height, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    };

    void run() {
        int prev_time = SDL_GetTicks();

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
            }

            int current_time = SDL_GetTicks();
            float delta_time = (current_time - prev_time) / 1000.0f;
            prev_time = current_time;

            static float dz = 1.0f;
            static float angle = 0.0f;

            angle += M_PI / 4 * delta_time;

            screen screen_display = {dz, angle, aspect_ratio, window_width,
                                     window_height};

            // render the background and do a clear
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

            for (int i = 0; i < positions.size(); i++) {
                auto pt = screen_display.position(positions[i]);

                SDL_Point p = {int(pt.x), int(pt.y)};

                SDL_RenderDrawPoints(renderer, &p, 4);
            }

            for (std::vector<int> f : faces) {
                for (int j = 0; j < f.size(); j++) {
                    vec vtx1 = positions[f[j]];
                    vec vtx2 = positions[f[(j + 1) % f.size()]];

                    auto tv1 = screen_display.position(vtx1);
                    auto tv2 = screen_display.position(vtx2);

                    SDL_RenderDrawLine(renderer, tv1.x, tv1.y, tv2.x, tv2.y);
                }
            }

            SDL_RenderPresent(renderer);
        }
    }

    void quit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    }

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    bool running = true;
};

#endif // !SDL_RENDER_H
