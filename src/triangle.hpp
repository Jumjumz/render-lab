#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "ndebug.h"
#include "sdl.h"
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <stdexcept>
#include <utility>
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

    VkPhysicalDevice physicalDevice;

    vk::raii::Context context;

    void initWindow() {
        appWindow.window_width = 1440;
        appWindow.aspect_ratio = 16.0 / 9.0;

        appWindow.init();
    };

    void initVulkan() {
        createInstance();
        pickPhysicalDevice();
    };

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
    };

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("No GPU with Vulkan Support found\n");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);

        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates;

        for (size_t i = 0; i < devices.size(); i++) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);
            uint score = 0;

            switch (properties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                score += 1000;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                score += 300;
                break;
            default:
                break;
            }

            score += properties.limits.maxImageDimension2D;
            candidates.insert(std::make_pair(score, devices[i]));
        }

        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Failed to find suitable GPU!");
        }
    };

    void mainLoop() {};

    void cleanUp() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        appWindow.destroy();
    };
};

#endif // !TRIANGLE_HPP
