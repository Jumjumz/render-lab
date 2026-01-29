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

    bool running = true;

    SDL_Window *sdl_window;

    SDL_Event event;

    void init();

    void destroy() const;

  private:
    uint window_height;
};

#endif // !WINDOW_H
