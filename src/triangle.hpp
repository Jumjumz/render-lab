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
#include <set>
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

    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    vk::raii::Context context;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
    };

    void initWindow() {
        appWindow.window_width = 1440;
        appWindow.aspect_ratio = 16.0 / 9.0;

        appWindow.init();
    };

    void initVulkan() {
        createInstance();
        pickPhysicalDevice();
        createLogicalDevice();
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

        // get the physical device base on score
        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Failed to find suitable GPU!");
        }
    };

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies();

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int> uniqueQueueFamilies = {indices.graphicsFamily,
                                             indices.presentFamily};

        float queuePriority = 1.0f;
        for (int queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo deviceQueueInfo{};
            deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueInfo.queueFamilyIndex = queueFamily;
            deviceQueueInfo.queueCount = 1;
            deviceQueueInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(deviceQueueInfo);
        }

        // check device features
        VkPhysicalDeviceFeatures features{};
        memset(&features, 0, sizeof(VkPhysicalDeviceFeatures));

        features.samplerAnisotropy = VK_TRUE;
        features.geometryShader = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &features;

        // get device extensions
        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
            VK_SUCCESS)
            throw std::runtime_error("Failed to Create Logical Device!");

        // get queue handles
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
    };

    QueueFamilyIndices findQueueFamilies() {
        QueueFamilyIndices indices;
        // check queue families
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                                 &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(
            queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                                 &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;

            if (indices.isComplete())
                break;
        }

        return indices;
    };

    void mainLoop() {};

    void cleanUp() {
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        appWindow.destroy();
    };
};

#endif // !TRIANGLE_HPP
