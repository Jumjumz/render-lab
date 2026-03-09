#include "vulkan_context.hpp"
#include "ndebug.hpp"

#include <SDL_stdinc.h>
#include <SDL_vulkan.h>
#include <map>
#include <set>

VulkanContext::VulkanContext(SDL_Window *window) : window(window) {
    createInstance();
    pickPhysicalDevice();
    createSurface();
    createLogicalDevice();
};

void VulkanContext::createInstance() {
    constexpr vk::ApplicationInfo appInfo{
        "Render Lab", VK_MAKE_VERSION(0, 0, 1), "Jumz Engine",
        VK_MAKE_VERSION(0, 0, 1), vk::ApiVersion13};

    uint32_t extensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(this->window, &extensionCount, nullptr);

    std::vector<const char *> extensions(extensionCount);
    if (SDL_Vulkan_GetInstanceExtensions(this->window, &extensionCount,
                                         extensions.data()) != SDL_TRUE)
        throw std::runtime_error("Required SDL extension not supported!");

    std::vector<char const *> requiredLayers;

    // enable validation layers
    if (enableValidationLayers)
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());

    auto layerProperties = this->context.enumerateInstanceLayerProperties();

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
    this->instance = vk::raii::Instance{this->context, instanceInfo, nullptr};

    VkInstance instance = *this->instance;
    VkSurfaceKHR surface = *this->surface;

    // create sdl vulkan surface
    if (SDL_Vulkan_CreateSurface(this->window, instance, &surface) != SDL_TRUE)
        throw std::runtime_error("Failed to create SDL surface!");

    this->surface = vk::raii::SurfaceKHR{this->instance, surface};
};

void VulkanContext::pickPhysicalDevice() {
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

void VulkanContext::createLogicalDevice() {
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
        featureChain{};

    auto &features = featureChain.get<vk::PhysicalDeviceFeatures2>();
    features.features.geometryShader = vk::True;
    features.features.samplerAnisotropy = vk::True;
    features.features.dualSrcBlend = vk::True;
    features.features.robustBufferAccess = vk::True;

    auto &dynamicRendering =
        featureChain.get<vk::PhysicalDeviceVulkan13Features>();
    dynamicRendering.dynamicRendering = vk::True;
    dynamicRendering.synchronization2 = vk::True;

    auto &dynamicState =
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    dynamicState.extendedDynamicState = vk::True;

    // get swapchains extensions
    const std::vector<const char *> deviceExtensions = {
        vk::KHRSwapchainExtensionName, vk::EXTExtendedDynamicState3ExtensionName};

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

void VulkanContext::findQueueFamilies() {
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

void VulkanContext::createSurface() {
    surfaceConfig();

    this->config.chosenFormat = this->config.formats[0];
    for (const auto &format : this->config.formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            this->config.chosenFormat = format;
            break;
        }
    }

    this->config.chosenPresentMode =
        vk::PresentModeKHR::eFifo; // V-Sync capped at 60fps
    // loop commented out as it takes too much resources in CPU (almost 70%)
    // for (const auto &mode : this->config.presentModes) {
    //     if (mode == vk::PresentModeKHR::eMailbox) {
    //         this->config.chosenPresentMode = mode;
    //         break;
    //     }
    // }

    // choose extent
    if (this->config.capabilities.currentExtent.width != UINT32_MAX) {
        this->config.chosenExtent = this->config.capabilities.currentExtent;
    } else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(this->window, &width, &height);

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

void VulkanContext::surfaceConfig() {
    this->config.capabilities =
        this->physicalDevice.getSurfaceCapabilitiesKHR(this->surface);

    this->config.formats =
        this->physicalDevice.getSurfaceFormatsKHR(this->surface);

    this->config.presentModes =
        this->physicalDevice.getSurfacePresentModesKHR(this->surface);
};
