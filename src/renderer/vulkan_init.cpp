#include "renderer/vulkan_init.hpp"

#include "ndebug.h"
#include <SDL_stdinc.h>
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

VulkanInit::VulkanInit() {
    initWindow();
    createInstance();
    pickPhysicalDevice();
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
    VkSurfaceKHR surface;

    // create sdl vulkan surface
    if (SDL_Vulkan_CreateSurface(this->appWindow.sdl_window, instance,
                                 &surface) != SDL_TRUE)
        throw std::runtime_error("Failed to create SDL surface!");

    this->surface = vk::raii::SurfaceKHR{this->instance, surface};
};

void VulkanInit::pickPhysicalDevice() {
    auto devices = this->instance.enumeratePhysicalDevices();

    if (devices.empty())
        throw std::runtime_error("Failed to find GPUs with Vulkan Support!");

    std::multimap<int, vk::raii::PhysicalDevice> candidates;

    for (const auto &device : devices) {
        auto properties = device.getProperties();
        auto features = device.getFeatures();
        uint32_t score = 0;

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        } else if (properties.deviceType ==
                   vk::PhysicalDeviceType::eIntegratedGpu) {
            score += 100;
        }

        score += properties.limits.maxImageDimension2D;

        if (!features.geometryShader)
            continue;

        candidates.insert(std::make_pair(score, device));
    };

    if (candidates.rbegin()->first > 0) {
        this->physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
};

void VulkanInit::findQueueFamilies() {
    std::vector<vk::QueueFamilyProperties> familyProperties =
        this->physicalDevice.getQueueFamilyProperties();

    for (size_t i = 0; i < familyProperties.size(); i++) {
        if (familyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            this->familyIndices.graphicsFamily = i;
        }

        vk::Bool32 present =
            this->physicalDevice.getSurfaceSupportKHR(i, this->surface);

        if (present)
            this->familyIndices.presentFamily = i;

        if (this->familyIndices.isComplete())
            break;
    }
};

void VulkanInit::createLogicalDevice() {
    findQueueFamilies();

    std::vector<vk::DeviceQueueCreateInfo> deviceQueueInfos;
    std::set<int> uniqueQueueFamilies = {
        this->familyIndices.graphicsFamily,
        this->familyIndices.presentFamily,
    };

    float queuePriority = 0.5f;
    for (const int queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo deviceQueueInfo{};
        deviceQueueInfo.queueFamilyIndex = queueFamily;
        deviceQueueInfo.queueCount = 1;
        deviceQueueInfo.pQueuePriorities = &queuePriority;

        deviceQueueInfos.push_back(deviceQueueInfo);
    }

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {};

    auto &features = featureChain.get<vk::PhysicalDeviceFeatures2>();
    features.features.geometryShader = VK_TRUE;
    features.features.samplerAnisotropy = VK_TRUE;

    auto &dynamicRendering =
        featureChain.get<vk::PhysicalDeviceVulkan13Features>();
    dynamicRendering.dynamicRendering = VK_TRUE;

    auto &dynamicState =
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    dynamicState.extendedDynamicState = VK_TRUE;

    // get swapchains extensions
    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    vk::DeviceCreateInfo deviceInfo{};
    deviceInfo.pNext = &features;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = deviceQueueInfos.data();
    deviceInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    this->device = vk::raii::Device{this->physicalDevice, deviceInfo};

    this->graphicsQueue = vk::raii::Queue{
        this->device, static_cast<uint32_t>(this->familyIndices.graphicsFamily),
        0};
};
