#include "render_rec.h"
#include "screen.h"
#include <SDL2/SDL.h>

const int SIZE = 8;

int main(int argc, char *argv[]) {
    int window_width = 1280;
    int window_height = 720;

    screen squares[SIZE] = {
        {0.5, 0.5, 1},   {0.5, -0.5, 1},   {-0.5, 0.5, 1},   {-0.5, -0.5, 1},
        {0.5, 0.5, 1.5}, {0.5, -0.5, 1.5}, {-0.5, 0.5, 1.5}, {-0.5, -0.5, 1.5}};

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

        static float dz = 0.0f;
        dz += 1 * delta_time; // speed * delta time

        // render the background and do a clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < SIZE; i++) {
            auto sq = square({squares[i].x, squares[i].y, squares[i].z + dz},
                             window_width, window_height, square_size);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &sq);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
