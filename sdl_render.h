#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include "SDL.h"
#include "mesh.h"
#include "screen.h"
#include "vect.h"
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

class sdl_render {
  public:
    uint window_width = 720;
    float aspect_ratio = 1.0f;

    void run(const std::shared_ptr<mesh> &shape) {
        initialize();

        points = shape->surface_interpolation(this->subdivision);
        lines = shape->grid(this->subdivision);

        uint prev_time = SDL_GetTicks();

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
            }

            uint current_time = SDL_GetTicks();
            float delta_time = (current_time - prev_time) / 1000.0f;
            prev_time = current_time;

            static float focal_point = 1.0f;
            static float angle = 0.0f;

            angle += M_PI / 6 * delta_time; // velocity (radians) * time elapsed

            screen screen_display = {focal_point, angle, aspect_ratio,
                                     window_width, window_height};

            // render the background and do a clear
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            // prepare the color for what we to draw in the current frame
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);

            // points in the surface (interpolate)
            for (size_t i = 0; i < points.size(); i++) {
                vect2 pt = screen_display.position(points[i]);

                SDL_RenderDrawPoint(renderer, pt.x(), pt.y());
            }

            // lines connecting points
            for (from_to f : lines) {
                for (size_t i = 0; i < f.size(); i++) {
                    vect2 pt_a = screen_display.position(points[f[i]]);
                    vect2 pt_b =
                        screen_display.position(points[f[(i + 1) % f.size()]]);

                    SDL_RenderDrawLine(renderer, pt_a.x(), pt_a.y(), pt_b.x(),
                                       pt_b.y());
                }
            }

            SDL_RenderPresent(renderer);
        }
    }

    void destroy() const {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    }

  private:
    uint window_height;
    bool running = true;
    uint subdivision = 8;

    std::vector<vect> points;
    std::vector<from_to> lines;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    void initialize() {
        window_height = uint(window_width / aspect_ratio);

        SDL_Init(SDL_INIT_EVERYTHING);

        window = SDL_CreateWindow("Wireframe Render", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, window_width,
                                  window_height, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
};

#endif // !SDL_RENDER_H
