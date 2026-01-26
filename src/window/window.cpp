#include "window.h"

void window::init() {
    this->window_height = uint(this->window_width / this->aspect_ratio);
    this->window_height =
        (this->window_height < 1)
            ? 1
            : this->window_height; // make sure height is not 0 or negative

    SDL_Init(SDL_INIT_VIDEO);

    this->sdl_window = SDL_CreateWindow(
        "Wireframe Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->window_width, this->window_height, SDL_WINDOW_VULKAN);
};

void window::run() {
    while (this->running) {
        while (SDL_PollEvent(&this->event)) {
            if (this->event.type == SDL_QUIT)
                this->running = false;
        }
    }
};

void window::destroy() const {
    SDL_DestroyWindow(this->sdl_window);

    SDL_Quit();
};
