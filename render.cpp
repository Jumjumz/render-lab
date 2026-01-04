#include "screen.h"
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    int window_width = 1280;
    int window_height = 720;

    screen s(0.5, 0.5, 1);
    screen p = project(s.x, s.y, s.z);
    screen d = display(p.x, p.y, window_width, window_height);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Wireframe Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;

    int square_size = 20;
    SDL_Rect rectangle = {offset(d.x, square_size), offset(d.y, square_size),
                          square_size, square_size};
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rectangle);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
