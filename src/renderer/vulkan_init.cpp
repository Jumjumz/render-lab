#include "renderer/vulkan_init.hpp"

#include "ndebug.h"
#include <SDL_stdinc.h>
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>

VulkanInit::VulkanInit() {
    initWindow();
    createInstance();
    physicalDevice();
    createLogicalDevice();
};

void VulkanInit::initWindow() {
    this->appWindow.window_width = 1440;
    this->appWindow.aspect_ratio = 16.0 / 9.0;

    this->appWindow.init();
};

void VulkanInit::createInstance() {
    constexpr vk::ApplicationInfo appInfo{"Rener Lab", VK_MAKE_VERSION(0, 0, 1),
                                          "Jumz Engine", VK_MAKE_VERSION(0, 0, 1),
                                          vk::ApiVersion13};

    uint32_t extensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(this->appWindow.sdl_window,
                                     &extensionCount, nullptr);
    std::vector<const char *> extensions(extensionCount);
    if (SDL_Vulkan_GetInstanceExtensions(this->appWindow.sdl_window,
                                         &extensionCount,
                                         extensions.data()) != SDL_TRUE)
        throw std::runtime_error("Required SDL extension not supported!");

    std::vector<char const *> requiredLayers;

    // enable validation layers
    if (enableValidationLayers)
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());

    vk::raii::Context context;
    auto layerProperties = context.enumerateInstanceLayerProperties();

    if (std::ranges::any_of(requiredLayers, [&layerProperties](
                                                const auto &requiredLayer) {
            return std::ranges::none_of(
                layerProperties, [requiredLayer](auto const &layerProperty) {
                    return strcmp(layerProperty.layerName, requiredLayer) == 0;
                });
        })) {
        throw std::runtime_error(
            "One or more required layers are not supported!");
    }

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers) {
        instanceInfo.enabledLayerCount =
            static_cast<uint32_t>(requiredLayers.size());
        instanceInfo.ppEnabledLayerNames = requiredLayers.data();
    }

    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    // create instance
    this->instance = vk::raii::Instance{context, instanceInfo, nullptr};

    VkInstance instance = *this->instance;
    VkSurfaceKHR surface = *this->surface;

    // create sdl vulkan surface
    if (SDL_Vulkan_CreateSurface(this->appWindow.sdl_window, instance,
                                 &surface) != SDL_TRUE)
        throw std::runtime_error("Failed to create sdl surface!");
};
