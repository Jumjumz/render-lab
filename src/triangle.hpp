#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "sdl.h"
#include <SDL_vulkan.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

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

    VkInstance instance;
    VkSurfaceKHR surface;

    void initWindow() {
        appWindow.window_width = 1440;
        appWindow.aspect_ratio = 16.0 / 9.0;

        appWindow.init();
    };

    void initVulkan() { createInstance(); };

    void createInstance() {
        constexpr vk::ApplicationInfo appInfo{
            "Wireframe Renderer", VK_MAKE_VERSION(1, 0, 0), "Jumz Engine",
            VK_MAKE_VERSION(0, 0, 1), vk::ApiVersion13};

        uint32_t extensionsCount = 0;

        SDL_Vulkan_GetInstanceExtensions(appWindow.window, &extensionsCount,
                                         nullptr);
        std::vector<const char *> extensions(extensionsCount);
        SDL_Vulkan_GetInstanceExtensions(appWindow.window, &extensionsCount,
                                         extensions.data());

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = extensions.data();

        vkCreateInstance(&createInfo, nullptr, &instance);

        SDL_Vulkan_CreateSurface(appWindow.window, instance, &surface);

        // run
        appWindow.run();
    }

    void mainLoop() {};
    void cleanUp() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        appWindow.destroy();
    };
};

#endif // !TRIANGLE_HPP
