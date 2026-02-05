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
    createSurface();
    createSwapChain();
    createViewImage();
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
    features.features.geometryShader = vk::True;
    features.features.samplerAnisotropy = vk::True;

    auto &dynamicRendering =
        featureChain.get<vk::PhysicalDeviceVulkan13Features>();
    dynamicRendering.dynamicRendering = vk::True;

    auto &dynamicState =
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    dynamicState.extendedDynamicState = vk::True;

    // get swapchains extensions
    const std::vector<const char *> deviceExtensions = {
        vk::KHRSwapchainExtensionName};

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

    this->presentQueue = vk::raii::Queue{
        this->device, static_cast<uint32_t>(this->familyIndices.presentFamily),
        0};
};

void VulkanInit::surfaceConfig() {
    this->config.capabilities =
        this->physicalDevice.getSurfaceCapabilitiesKHR(this->surface);

    this->config.formats =
        this->physicalDevice.getSurfaceFormatsKHR(this->surface);

    this->config.presentModes =
        this->physicalDevice.getSurfacePresentModesKHR(this->surface);
};

void VulkanInit::createSurface() {
    surfaceConfig();

    this->config.chosenFormat = this->config.formats[0];
    for (const auto &format : this->config.formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            this->config.chosenFormat = format;
            break;
        }
    }

    this->config.chosenPresentMode = vk::PresentModeKHR::eFifo;
    for (const auto &mode : this->config.presentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            this->config.chosenPresentMode = mode;
            break;
        }
    }

    // choose extent
    if (this->config.capabilities.currentExtent.width != UINT32_MAX) {
        this->config.chosenExtent = this->config.capabilities.currentExtent;
    } else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(this->appWindow.sdl_window, &width, &height);

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

void VulkanInit::createSwapChain() {
    vk::SwapchainCreateInfoKHR chainInfo{};
    chainInfo.flags = vk::SwapchainCreateFlagsKHR();
    chainInfo.surface = this->surface;
    chainInfo.minImageCount = this->config.imageCount;
    chainInfo.imageFormat = this->config.chosenFormat.format;
    chainInfo.imageColorSpace = this->config.chosenFormat.colorSpace;
    chainInfo.imageExtent = this->config.chosenExtent;
    chainInfo.imageArrayLayers = 1;
    chainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queueFamilyIndeces[] = {
        static_cast<uint32_t>(this->familyIndices.graphicsFamily),
        static_cast<uint32_t>(this->familyIndices.presentFamily)};

    if (this->familyIndices.graphicsFamily != this->familyIndices.presentFamily) {
        chainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        chainInfo.queueFamilyIndexCount = 2;
        chainInfo.pQueueFamilyIndices = queueFamilyIndeces;
    } else {
        chainInfo.imageSharingMode = vk::SharingMode::eExclusive;
        chainInfo.queueFamilyIndexCount = 0;
        chainInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
    }

    chainInfo.preTransform = this->config.capabilities.currentTransform;
    chainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    chainInfo.presentMode = this->config.chosenPresentMode;
    chainInfo.clipped = vk::True;
    chainInfo.oldSwapchain = VK_NULL_HANDLE;

    this->swapChain = vk::raii::SwapchainKHR{this->device, chainInfo};
    this->swapchainResources.images = this->swapChain.getImages();
};

void VulkanInit::createViewImage() {
    this->swapchainResources.imageViews.clear();

    for (auto const &image : this->swapchainResources.images) {
        vk::ImageViewCreateInfo imageInfo{};
        imageInfo.image = image;
        imageInfo.format = this->config.chosenFormat.format;
        imageInfo.viewType = vk::ImageViewType::e2D;
        imageInfo.components.r = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imageInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                      1};

        this->swapchainResources.imageViews.emplace_back(this->device, imageInfo);
    }

    this->swapchainResources.extent = this->config.chosenExtent;
    this->swapchainResources.imageFormat = this->config.chosenFormat.format;
};
