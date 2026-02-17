#include "renderer/render.hpp"
#include "renderer/render_lab.hpp"
#include "shapes/cube.hpp"
#include "utils/resolution.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>

int main() {
    auto reso = Resolution::FHD;
    auto aspect = Aspect::R16_9;

    const Render shape{std::make_shared<Cube>(0.5)};

    RenderLab app{reso, aspect, shape};

    try {
        app.run();
    } catch (const vk::SystemError &err) {
        std::cerr << "Vulkan Error: " << err.what() << std::endl;

        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}
