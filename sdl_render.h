#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include "SDL.h"
#include "SDL_render.h"
#include "mesh.h"
#include "screen.h"
#include "vect.h"
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

class sdl_render {
  public:
    uint32_t window_width = 720;
    float aspect_ratio = 1.0f;

    void run(const std::shared_ptr<mesh> &shape) {
        initialize();

        points = shape->points();
        // lines = shape->lines();
        surface_points = shape->surface_interpolation(this->subdivisions);
        surface_lines = shape->surface_lines(this->subdivisions);

        uint32_t prev_time = SDL_GetTicks();

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
            }

            uint32_t current_time = SDL_GetTicks();
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

            // vertices
            for (int i = 0; i < points.size(); i++) {
                vect2 pt = screen_display.position(points[i]);

                SDL_RenderDrawPoint(renderer, pt.x(), pt.y());
            }

            // lines connecting vertices
            for (from_to f : lines) {
                for (int j = 0; j < f.size(); j++) {
                    vect2 pt_a = screen_display.position(points[f[j]]);
                    vect2 pt_b =
                        screen_display.position(points[f[(j + 1) % f.size()]]);

                    SDL_RenderDrawLine(renderer, pt_a.x(), pt_a.y(), pt_b.x(),
                                       pt_b.y());
                }
            }

            // points in the surface (interpolate)
            for (int i = 0; i < surface_points.size(); i++) {
                vect2 pt = screen_display.position(surface_points[i]);

                SDL_RenderDrawPoint(renderer, pt.x(), pt.y());
            }

            for (from_to f : surface_lines) {
                for (int i = 0; i < f.size(); i++) {
                    vect2 pt_a = screen_display.position(surface_points[f[i]]);
                    vect2 pt_b = screen_display.position(
                        surface_points[f[(i + 1) % f.size()]]);

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
    uint32_t window_height;
    bool running = true;
    uint32_t subdivisions = 5;

    std::vector<vect> points;
    std::vector<from_to> lines;
    std::vector<vect> surface_points;
    std::vector<from_to> surface_lines;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    void initialize() {
        window_height = uint32_t(window_width / aspect_ratio);

        SDL_Init(SDL_INIT_EVERYTHING);

        window = SDL_CreateWindow("Wireframe Render", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, window_width,
                                  window_height, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
};

#endif // !SDL_RENDER_H
