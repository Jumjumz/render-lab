#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "sdl.h"
#include <vulkan/vulkan_raii.hpp>

class HelloTriangleApplication {
  public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

  private:
    sdl appWindow;

    void initWindow() {
        appWindow.window_width = 1440;
        appWindow.aspect_ratio = 16.0 / 9.0;

        appWindow.run();
    };

    void initVulkan() {};
    void mainLoop() {};
    void cleanUp() { appWindow.destroy(); };
};

#endif // !TRIANGLE_HPP
