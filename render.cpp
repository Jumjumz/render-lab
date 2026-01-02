#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char *argv[]) {
    int window_width = 800;
    int window_height = 600;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;

        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, window_width,
                                          window_height, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;

        return 1;
    }

    SDL_Delay(10000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
