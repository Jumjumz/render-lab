#include "render_rec.h"
#include "screen.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

const int SIZE = 8;

int main(int argc, char *argv[]) {
    int window_width = 1280;
    int window_height = 720;

    screen squares[SIZE] = {{0.25, 0.25, 0.25},    {-0.25, 0.25, 0.25},
                            {-0.25, -0.25, 0.25},  {0.25, -0.25, 0.25},
                            {0.25, 0.25, -0.25},   {-0.25, 0.25, -0.25},
                            {-0.25, -0.25, -0.25}, {0.25, -0.25, -0.25}};

    std::vector<std::vector<int>> fs = {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4},
                                        {1, 5},       {2, 6},       {3, 7}};

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Wireframe Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;

    int square_size = 20;
    bool running = true;
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
        static float angle = 0;

        // dz += 1 * delta_time; // speed * delta time
        angle += M_PI / 4 * delta_time;

        // render the background and do a clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        for (int i = 0; i < SIZE; i++) {
            auto t = translate_z(rotate(squares[i], angle), dz);
            auto sq = square(t, window_width, window_height, square_size);
            auto pt = point(t, window_width, window_height);

            // SDL_RenderFillRect(renderer, &sq);
            SDL_RenderDrawPoints(renderer, &pt, 4);
            // SDL_RenderDrawLines(renderer, &pt, SIZE);
        }

        for (std::vector<int> f : fs) {
            for (int j = 0; j < f.size(); j++) {
                screen vtx1 = squares[f[j]];
                screen vtx2 = squares[f[(j + 1) % f.size()]];

                auto tv1 = translate_z(rotate(vtx1, angle), dz);
                auto tv2 = translate_z(rotate(vtx2, angle), dz);

                auto vt1 = vertex(tv1, window_width, window_height);
                auto vt2 = vertex(tv2, window_width, window_height);

                SDL_RenderDrawLine(renderer, vt1.x, vt1.y, vt2.x, vt2.y);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
