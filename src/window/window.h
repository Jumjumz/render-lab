#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <SDL.h>
#include <sys/types.h>

class window {
  public:
    window() = default;

    uint window_width = 720;
    float aspect_ratio = 1.0;

    SDL_Window *sdl_window;

    void init();

    void run();

    void destroy() const;

  private:
    uint window_height;
    bool running = true;

    SDL_Event event;
};

#endif // !WINDOW_H
