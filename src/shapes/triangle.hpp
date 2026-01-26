#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "../ndebug.h"
#include "../sdl.h"
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <stdexcept>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

class Triangle {
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
    VkSwapchainKHR swapChain;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const {
            return graphicsFamily >= 0 && presentFamily >= 0;
        };
    } indices;

    struct SurfaceConfig {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        VkSurfaceFormatKHR chosenFormat;
        VkPresentModeKHR chosenPresentMode;
        VkExtent2D chosenExtent;

        uint32_t imageCount;
    } config;

    struct SwapchainResources {
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

        VkFormat imageFormat;
        VkExtent2D extent;
    } resources;

    void initWindow() {
        this->appWindow.window_width = 1440;
        this->appWindow.aspect_ratio = 16.0 / 9.0;

        this->appWindow.init();
    };

    void initVulkan() {
        createInstance();
        pickPhysicalDevice();
        createSurface();
        createLogicalDevice();
        createSwapChain();
        createViewImage();
        createGraphicsPipeline();
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

        const vk::raii::Context context;

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

        SDL_Vulkan_GetInstanceExtensions(this->appWindow.window,
                                         &extensionsCount, nullptr);
        std::vector<const char *> extensions(extensionsCount);
        SDL_Vulkan_GetInstanceExtensions(this->appWindow.window,
                                         &extensionsCount, extensions.data());

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.enabledLayerCount =
            static_cast<uint32_t>(requiredLayers.size());
        instanceInfo.ppEnabledLayerNames = requiredLayers.data();
        instanceInfo.enabledExtensionCount = extensionsCount;
        instanceInfo.ppEnabledExtensionNames = extensions.data();

        vkCreateInstance(&instanceInfo, nullptr, &instance);

        // create sdl vulkan surface
        SDL_Vulkan_CreateSurface(this->appWindow.window, this->instance,
                                 &this->surface);

        // run
        this->appWindow.run();
    };

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("No GPU with Vulkan support found\n");

        std::vector<VkPhysicalDevice> devices(deviceCount);

        vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices.data());

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
            this->physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Failed to find suitable GPU!");
        }
    };

    void createLogicalDevice() {
        findQueueFamilies(); // init indices

        std::vector<VkDeviceQueueCreateInfo> deviceQueueInfos;
        std::set<int> uniqueQueueFamilies = {this->indices.graphicsFamily,
                                             this->indices.presentFamily};

        float queuePriority = 1.0f;
        for (const int queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo deviceQueueInfo{};
            deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueInfo.queueFamilyIndex = queueFamily;
            deviceQueueInfo.queueCount = 1;
            deviceQueueInfo.pQueuePriorities = &queuePriority;

            deviceQueueInfos.push_back(deviceQueueInfo);
        }

        // check device features
        VkPhysicalDeviceFeatures features{};
        features.samplerAnisotropy = VK_TRUE;
        features.geometryShader = VK_TRUE;

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pQueueCreateInfos = deviceQueueInfos.data();
        deviceInfo.queueCreateInfoCount =
            static_cast<uint32_t>(deviceQueueInfos.size());
        deviceInfo.pEnabledFeatures = &features;

        // get swapchains extensions
        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        deviceInfo.enabledExtensionCount =
            static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr,
                           &this->device) != VK_SUCCESS)
            throw std::runtime_error("Failed to create logical device!");

        // get queue handles
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        vkGetDeviceQueue(this->device, this->indices.graphicsFamily, 0,
                         &graphicsQueue);
        vkGetDeviceQueue(this->device, this->indices.presentFamily, 0,
                         &presentQueue);
    };

    void findQueueFamilies() {
        // check queue families
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice,
                                                 &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(
            queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice,
                                                 &queueFamilyCount,
                                                 queueFamilyProperties.data());

        for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                this->indices.graphicsFamily = i;

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i,
                                                 this->surface, &presentSupport);

            if (presentSupport)
                this->indices.presentFamily = i;

            if (this->indices.isComplete())
                break;
        }
    };

    void createSurface() {
        surfaceConfig(); // init config

        this->config.chosenFormat = this->config.formats[0]; // default
        for (const auto &format : this->config.formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                this->config.chosenFormat = format;
                break;
            }
        }

        this->config.chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (const auto &mode : this->config.presentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                this->config.chosenPresentMode = mode;
                break;
            }
        }

        // choose extent
        if (this->config.capabilities.currentExtent.width != UINT32_MAX) {
            this->config.chosenExtent = this->config.capabilities.currentExtent;
        } else {
            int width, height;
            SDL_Vulkan_GetDrawableSize(this->appWindow.window, &width, &height);

            this->config.chosenExtent.width =
                std::clamp(static_cast<uint32_t>(width),
                           this->config.capabilities.minImageExtent.width,
                           this->config.capabilities.maxImageExtent.width);

            this->config.chosenExtent.height =
                std::clamp(static_cast<uint32_t>(height),
                           this->config.capabilities.minImageExtent.height,
                           this->config.capabilities.maxImageExtent.height);
        }

        this->config.imageCount = this->config.capabilities.minImageCount + 1;
        if (this->config.capabilities.maxImageCount > 0 &&
            this->config.imageCount > this->config.capabilities.maxImageCount) {
            this->config.imageCount = this->config.capabilities.maxImageCount;
        }
    };

    void surfaceConfig() {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            this->physicalDevice, this->surface, &this->config.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface,
                                             &formatCount, nullptr);

        this->config.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice,
                                             this->surface, &formatCount,
                                             this->config.formats.data());

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            this->physicalDevice, this->surface, &presentModeCount, nullptr);

        this->config.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            this->physicalDevice, this->surface, &presentModeCount,
            this->config.presentModes.data());
    }

    void createSwapChain() {
        VkSwapchainCreateInfoKHR swapInfo{};
        swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapInfo.surface = this->surface;
        swapInfo.minImageCount = this->config.imageCount;
        swapInfo.imageFormat = this->config.chosenFormat.format;
        swapInfo.imageColorSpace = this->config.chosenFormat.colorSpace;
        swapInfo.imageExtent = this->config.chosenExtent;
        swapInfo.imageArrayLayers = 1;
        swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndeces[] = {
            static_cast<uint32_t>(this->indices.graphicsFamily),
            static_cast<uint32_t>(this->indices.presentFamily)};

        if (this->indices.graphicsFamily != this->indices.presentFamily) {
            swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapInfo.queueFamilyIndexCount = 2;
            swapInfo.pQueueFamilyIndices = queueFamilyIndeces;
        } else {
            swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        swapInfo.preTransform = this->config.capabilities.currentTransform;
        swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapInfo.presentMode = this->config.chosenPresentMode;
        swapInfo.clipped = VK_TRUE;
        swapInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->device, &swapInfo, nullptr,
                                 &this->swapChain) != VK_SUCCESS)
            throw std::runtime_error("Failed to create swapchain!");
    };

    void createViewImage() {
        uint32_t imageCount;
        vkGetSwapchainImagesKHR(this->device, this->swapChain, &imageCount,
                                nullptr);
        this->resources.images.resize(imageCount);
        vkGetSwapchainImagesKHR(this->device, this->swapChain, &imageCount,
                                this->resources.images.data());

        this->resources.imageViews.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = this->resources.images[i];
            viewInfo.format = this->config.chosenFormat.format;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(this->device, &viewInfo, nullptr,
                                  &this->resources.imageViews[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create view info!");
        }

        this->resources.imageFormat = this->config.chosenFormat.format;
        this->resources.extent = this->config.chosenExtent;
    };

    void createGraphicsPipeline() {};

    void mainLoop() {};

    void cleanUp() const {
        for (const auto imageView : this->resources.imageViews) {
            vkDestroyImageView(this->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
        vkDestroyDevice(this->device, nullptr);
        vkDestroyInstance(this->instance, nullptr);

        this->appWindow.destroy();
    };
};

#endif // !TRIANGLE_HPP
