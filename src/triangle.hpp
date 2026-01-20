#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "ndebug.h"
#include "sdl.h"
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>
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

    vk::raii::Context context;

    void initWindow() {
        appWindow.window_width = 1440;
        appWindow.aspect_ratio = 16.0 / 9.0;

        appWindow.init();
    };

    void initVulkan() { createInstance(); };

    void createInstance() {
        // create app information
        VkApplicationInfo appInfo{};
        appInfo.pApplicationName = "Wireframe Render";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = "Jumz Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = vk::ApiVersion13;

        // enable validation layers
        std::vector<char const *> requiredLayers;

        if (enableValidationLayers)
            requiredLayers.assign(validationLayers.begin(),
                                  validationLayers.end());

        auto layerProperties = context.enumerateInstanceLayerProperties();

        if (std::ranges::any_of(requiredLayers, [&layerProperties](
                                                    auto const &requiredLayer) {
                return std::ranges::none_of(
                    layerProperties, [requiredLayer](auto const &layerPropery) {
                        return strcmp(layerPropery.layerName, requiredLayer) == 0;
                    });
            })) {
            throw std::runtime_error(
                "One or more required layers are not supported!");
        }

        // get SDL vulkan extensions
        uint32_t extensionsCount = 0;

        SDL_Vulkan_GetInstanceExtensions(appWindow.window, &extensionsCount,
                                         nullptr);
        std::vector<const char *> extensions(extensionsCount);
        SDL_Vulkan_GetInstanceExtensions(appWindow.window, &extensionsCount,
                                         extensions.data());

        VkInstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data();
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = extensions.data();

        vkCreateInstance(&createInfo, nullptr, &instance);

        // create sdl vulkan surface
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
