#include "triangle.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const vk::SystemError &err) {
        std::cerr << "Vulkan Error: " << err.what() << std::endl;

        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
