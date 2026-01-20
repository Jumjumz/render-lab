#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <vulkan/vulkan_raii.hpp>

class HelloTriangleApplication {
  public:
    void run() {
        initVulkan();
        mainLoop();
        cleanUp();
    }

  private:
    void initVulkan() {};
    void mainLoop() {};
    void cleanUp() {};
};

#endif // !TRIANGLE_HPP
