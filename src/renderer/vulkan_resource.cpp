#include "vulkan_resource.hpp"

#include "ndebug.h"
#include <SDL_stdinc.h>
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

VulkanResource::VulkanResource() {
    initWindow();
    createInstance();
    pickPhysicalDevice();
    createSurface();
    createLogicalDevice();
    createSwapChain();
    createViewImage();
    createGraphicsPipeline();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
};

void VulkanResource::run() {
    mainLoop();
    cleanUp();
};

void VulkanResource::initWindow() {
    this->appWindow.window_width = 1440;
    this->appWindow.aspect_ratio = 16.0 / 9.0;

    this->appWindow.init();
};

void VulkanResource::createInstance() {
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

    this->vkInstance = *this->instance;

    // create sdl vulkan surface
    if (SDL_Vulkan_CreateSurface(this->appWindow.sdl_window, this->vkInstance,
                                 &this->vkSurface) != SDL_TRUE)
        throw std::runtime_error("Failed to create SDL surface!");

    this->surface = vk::raii::SurfaceKHR{this->instance, this->vkSurface};
};

void VulkanResource::pickPhysicalDevice() {
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

void VulkanResource::findQueueFamilies() {
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

void VulkanResource::createLogicalDevice() {
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

void VulkanResource::surfaceConfig() {
    this->config.capabilities =
        this->physicalDevice.getSurfaceCapabilitiesKHR(this->surface);

    this->config.formats =
        this->physicalDevice.getSurfaceFormatsKHR(this->surface);

    this->config.presentModes =
        this->physicalDevice.getSurfacePresentModesKHR(this->surface);
};

void VulkanResource::createSurface() {
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

void VulkanResource::createSwapChain() {
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
        chainInfo.pQueueFamilyIndices = nullptr;
    }

    chainInfo.preTransform = this->config.capabilities.currentTransform;
    chainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    chainInfo.presentMode = this->config.chosenPresentMode;
    chainInfo.clipped = vk::True;
    chainInfo.oldSwapchain = nullptr;

    this->swapChain = vk::raii::SwapchainKHR{this->device, chainInfo};
    this->resources.images = this->swapChain.getImages();
};

void VulkanResource::createViewImage() {
    this->resources.imageViews.clear();

    for (auto const &image : this->resources.images) {
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

        this->resources.imageViews.emplace_back(this->device, imageInfo);
    }

    this->resources.extent = this->config.chosenExtent;
    this->resources.imageFormat = this->config.chosenFormat.format;
};

[[nodiscard]]
vk::raii::ShaderModule
VulkanResource::createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo shaderInfo{};
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    vk::raii::ShaderModule shaderModule{this->device, shaderInfo};

    return shaderModule;
};

std::vector<char> VulkanResource::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
};

void VulkanResource::createGraphicsPipeline() {
    auto shaderModule = createShaderModule(readFile("shaders/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "vertMain";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = shaderModule;
    fragShaderStageInfo.pName = "fragMain";

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{};
    assemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    vk::PipelineVertexInputStateCreateInfo vertexInfo{};
    vertexInfo.vertexBindingDescriptionCount = 0;
    vertexInfo.vertexAttributeDescriptionCount = 0;
    vertexInfo.pVertexBindingDescriptions = nullptr;
    vertexInfo.pVertexAttributeDescriptions = nullptr;

    vk::PipelineViewportStateCreateInfo viewportStateInfo{};
    viewportStateInfo.pViewports = nullptr; // use dynamic viewport state
    viewportStateInfo.pScissors = nullptr;  // use dunamic scissor state
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.depthClampEnable = vk::False;
    rasterizationInfo.rasterizerDiscardEnable = vk::False;
    rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
    rasterizationInfo.depthBiasEnable = vk::False;
    rasterizationInfo.depthBiasSlopeFactor = 1.0f;
    rasterizationInfo.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multiSamplingInfo{};
    multiSamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multiSamplingInfo.sampleShadingEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.blendEnable = vk::False;
    colorAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorAttachment.srcColorBlendFactor = vk::BlendFactor::eSrc1Alpha;
    colorAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrc1Alpha;
    colorAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.logicOp = vk::LogicOp::eCopy;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorAttachment;

    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pushConstantRangeCount = 0;

    this->layout = vk::raii::PipelineLayout{this->device, layoutInfo, nullptr};

    vk::PipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &this->resources.imageFormat;

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pNext = &renderingInfo;
    pipelineInfo.pVertexInputState = &vertexInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportStateInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multiSamplingInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = this->layout;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    this->graphicsPipeline =
        vk::raii::Pipeline{this->device, nullptr, pipelineInfo, nullptr};
};

void VulkanResource::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex =
        static_cast<uint32_t>(this->familyIndices.graphicsFamily);

    this->commandPool = vk::raii::CommandPool{this->device, poolInfo, nullptr};
};

void VulkanResource::createCommandBuffers() {
    this->commandBuffers.clear();

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount = VulkanResource::MAX_FRAMES_IN_FLIGHT;

    this->commandBuffers = vk::raii::CommandBuffers{this->device, allocInfo};
};

void VulkanResource::drawFrame() {
    auto fenceResult = this->device.waitForFences(
        *this->inFlightFences[this->currentFrame], vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence!");
    }

    auto [result, imageIndex] = this->swapChain.acquireNextImage(
        UINT64_MAX, *this->availableSemaphores[this->currentFrame], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess &&
               result != vk::Result::eSuboptimalKHR) {
        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    this->device.resetFences(*this->inFlightFences[this->currentFrame]);

    this->commandBuffers[this->currentFrame].reset();

    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags destinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1,
    submitInfo.pWaitSemaphores = &*this->availableSemaphores[this->currentFrame];
    submitInfo.pWaitDstStageMask = &destinationStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*this->commandBuffers[this->currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &*this->finishedSemaphores[this->currentFrame];

    this->graphicsQueue.submit(submitInfo,
                               *this->inFlightFences[this->currentFrame]);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &*this->finishedSemaphores[this->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &*this->swapChain;
    presentInfo.pImageIndices = &imageIndex;

    result = this->presentQueue.presentKHR(presentInfo);

    if ((result == vk::Result::eSuboptimalKHR) ||
        (result == vk::Result::eErrorOutOfDateKHR) || framebufferResized) {
        this->framebufferResized = false;
        recreateSwapChain();
    } else {
        assert(result == vk::Result::eSuccess);
    }

    this->currentFrame =
        (this->currentFrame + 1) % VulkanResource::MAX_FRAMES_IN_FLIGHT;
};

void VulkanResource::transitionImageLayout(
    uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask) {
    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcStageMask = srcStageMask;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstStageMask = dstStageMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.image = this->resources.images[imageIndex];
    barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    this->commandBuffers[this->currentFrame].pipelineBarrier2(dependencyInfo);
};

void VulkanResource::recordCommandBuffer(uint32_t imageIndex) {
    auto &cmd = this->commandBuffers[this->currentFrame];

    cmd.begin({});

    transitionImageLayout(imageIndex, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eColorAttachmentOptimal, {},
                          vk::AccessFlagBits2::eColorAttachmentWrite,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    // set up color attachment
    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderingAttachmentInfo attachmentInfo{};
    attachmentInfo.imageView = this->resources.imageViews[imageIndex];
    attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentInfo.clearValue = clearColor;

    vk::Offset2D offset = {0, 0};

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea.offset = offset;
    renderingInfo.renderArea.extent = this->resources.extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &attachmentInfo;

    cmd.beginRendering(renderingInfo);

    // render commands
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, this->graphicsPipeline);
    cmd.setViewport(
        0, vk::Viewport{
               0.0f, 0.0f, static_cast<float>(this->resources.extent.width),
               static_cast<float>(this->resources.extent.height), 0.0f, 1.0f});

    cmd.setScissor(0, vk::Rect2D{vk::Offset2D{0, 0}, this->resources.extent});

    cmd.draw(3, 1, 0, 0);

    cmd.endRendering();

    transitionImageLayout(imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ImageLayout::ePresentSrcKHR,
                          vk::AccessFlagBits2::eColorAttachmentWrite, {},
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eBottomOfPipe);

    cmd.end();
};

void VulkanResource::createSyncObjects() {
    assert(this->availableSemaphores.empty() &&
           this->finishedSemaphores.empty() && this->inFlightFences.empty());

    for (size_t i = 0; i < this->resources.images.size(); i++) {
        this->finishedSemaphores.emplace_back(this->device,
                                              vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < VulkanResource::MAX_FRAMES_IN_FLIGHT; i++) {
        this->availableSemaphores.emplace_back(this->device,
                                               vk::SemaphoreCreateInfo());

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        this->inFlightFences.emplace_back(this->device, fenceInfo);
    }
};

void VulkanResource::mainLoop() {
    while (this->appWindow.running) {
        while (SDL_PollEvent(&this->appWindow.event)) {
            if (this->appWindow.event.type == SDL_QUIT)
                this->appWindow.running = false;

            if (this->appWindow.event.type == SDL_WINDOWEVENT_RESIZED)
                this->framebufferResized = true;
        }

        drawFrame();
    }

    this->device.waitIdle();
};

void VulkanResource::recreateSwapChain() {
    this->device.waitIdle();

    cleanupSwapChain();

    this->createSwapChain();
    this->createViewImage();
};

void VulkanResource::cleanupSwapChain() {
    this->resources.imageViews.clear();
    this->swapChain = nullptr;
};

void VulkanResource::cleanUp() {
    cleanupSwapChain();

    vkDestroySurfaceKHR(this->vkInstance, this->vkSurface, nullptr);
    vkDestroyInstance(this->vkInstance, nullptr);

    this->appWindow.destroy();
};
