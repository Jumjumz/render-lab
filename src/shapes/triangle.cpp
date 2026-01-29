#include "triangle.hpp"

#include "ndebug.h"
#include <SDL_vulkan.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

Triangle::Triangle() {
    this->instance = VK_NULL_HANDLE;
    this->surface = VK_NULL_HANDLE;

    this->device = VK_NULL_HANDLE;
    this->physicalDevice = VK_NULL_HANDLE;

    this->graphicsQueue = VK_NULL_HANDLE;
    this->presentQueue = VK_NULL_HANDLE;
    this->swapChain = VK_NULL_HANDLE;

    this->module = VK_NULL_HANDLE;
    this->renderPass = VK_NULL_HANDLE;

    this->graphicsPipeline = VK_NULL_HANDLE;
    this->layout = VK_NULL_HANDLE;

    this->commandPool = VK_NULL_HANDLE;
    this->commandBuffer = VK_NULL_HANDLE;
}

void Triangle::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanUp();
};

// private methods
void Triangle::initWindow() {
    this->appWindow.window_width = 1440;
    this->appWindow.aspect_ratio = 16.0 / 9.0;

    this->appWindow.init();
};

void Triangle::initVulkan() {
    createInstance();
    pickPhysicalDevice();
    createSurface();
    createLogicalDevice();
    createSwapChain();
    createViewImage();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
};

void Triangle::createInstance() {
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
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());

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

    SDL_Vulkan_GetInstanceExtensions(this->appWindow.sdl_window,
                                     &extensionsCount, nullptr);
    std::vector<const char *> extensions(extensionsCount);
    SDL_Vulkan_GetInstanceExtensions(this->appWindow.sdl_window,
                                     &extensionsCount, extensions.data());

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
    instanceInfo.ppEnabledLayerNames = requiredLayers.data();
    instanceInfo.enabledExtensionCount = extensionsCount;
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    vkCreateInstance(&instanceInfo, nullptr, &instance);

    // create sdl vulkan surface
    SDL_Vulkan_CreateSurface(this->appWindow.sdl_window, this->instance,
                             &this->surface);
};

void Triangle::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("No GPU with Vulkan support found!\n");

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
}

void Triangle::createLogicalDevice() {
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

    vkGetDeviceQueue(this->device, this->indices.graphicsFamily, 0,
                     &this->graphicsQueue);
    vkGetDeviceQueue(this->device, this->indices.presentFamily, 0,
                     &this->presentQueue);
};

void Triangle::findQueueFamilies() {
    // check queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice,
                                             &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        this->physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

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

void Triangle::createSurface() {
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

void Triangle::surfaceConfig() {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        this->physicalDevice, this->surface, &this->config.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface,
                                         &formatCount, nullptr);

    this->config.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface,
                                         &formatCount,
                                         this->config.formats.data());

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        this->physicalDevice, this->surface, &presentModeCount, nullptr);

    this->config.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice,
                                              this->surface, &presentModeCount,
                                              this->config.presentModes.data());
};

void Triangle::createSwapChain() {
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

void Triangle::createViewImage() {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(this->device, this->swapChain, &imageCount, nullptr);
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

void Triangle::createGraphicsPipeline() {
    VkShaderModule shaderModule =
        createShaderModule(readFile("shaders/slang.spv"));

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "vertMain";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = shaderModule;
    fragShaderStageInfo.pName = "fragMain";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInfo{};
    vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInfo.vertexBindingDescriptionCount = 0;
    vertexInfo.vertexAttributeDescriptionCount = 0;
    vertexInfo.pVertexBindingDescriptions = nullptr;
    vertexInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pViewports = nullptr; // use dynamic viewport state
    viewportState.pScissors = nullptr;  // use dunamic scissor state
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasSlopeFactor = 1.0f;
    rasterizer.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multiSampling{};
    multiSampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampling.sampleShadingEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.blendEnable = VK_FALSE;
    colorAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorAttachment;

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(this->device, &layoutInfo, nullptr,
                               &this->layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout!");

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &this->resources.imageFormat;

    VkGraphicsPipelineCreateInfo graphicsInfo{};
    graphicsInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsInfo.stageCount = 2;
    graphicsInfo.pStages = shaderStages;
    graphicsInfo.pNext = &renderingInfo;
    graphicsInfo.pVertexInputState = &vertexInfo;
    graphicsInfo.pInputAssemblyState = &inputAssembly;
    graphicsInfo.pViewportState = &viewportState;
    graphicsInfo.pRasterizationState = &rasterizer;
    graphicsInfo.pMultisampleState = &multiSampling;
    graphicsInfo.pColorBlendState = &colorBlending;
    graphicsInfo.pDynamicState = &dynamicState;
    graphicsInfo.layout = this->layout;
    graphicsInfo.renderPass = this->renderPass;
    graphicsInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &graphicsInfo,
                                  nullptr, &this->graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create a graphics pipeline!");
};

std::vector<char> Triangle::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    return buffer;
};

[[nodiscard]]
VkShaderModule Triangle::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(this->device, &shaderInfo, nullptr,
                             &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module!");

    this->module = shaderModule; // wip

    return shaderModule;
};

void Triangle::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = this->resources.imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderInfo{};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = 1;
    renderInfo.pAttachments = &colorAttachment;
    renderInfo.subpassCount = 1;
    renderInfo.pSubpasses = &subpass;
    renderInfo.dependencyCount = 1;
    renderInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(this->device, &renderInfo, nullptr,
                           &this->renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass!");
};

void Triangle::createFrameBuffers() {
    framebuffers.resize(this->resources.imageViews.size());

    for (size_t i = 0; i < this->resources.imageViews.size(); i++) {
        VkImageView attachments[] = {this->resources.imageViews[i]};

        VkFramebufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        bufferInfo.pAttachments = attachments;
        bufferInfo.renderPass = this->renderPass;
        bufferInfo.width = this->resources.extent.width;
        bufferInfo.height = this->resources.extent.height;
        bufferInfo.attachmentCount = 1;
        bufferInfo.layers = 1;

        if (vkCreateFramebuffer(this->device, &bufferInfo, nullptr,
                                &this->framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create a framebuffers!");
    }
};

void Triangle::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex =
        static_cast<uint32_t>(this->indices.graphicsFamily);

    if (vkCreateCommandPool(this->device, &poolInfo, nullptr,
                            &this->commandPool) != VK_SUCCESS)
        throw std::runtime_error("Faild to create command pool!");
};

void Triangle::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(this->device, &allocInfo,
                                 &this->commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer");
};

void Triangle::recordCommandBuffer(uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(this->commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderBeginInfo{};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = this->renderPass;
    renderBeginInfo.framebuffer = this->framebuffers[imageIndex];
    renderBeginInfo.renderArea.offset = {0, 0};
    renderBeginInfo.renderArea.extent = this->resources.extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(this->commandBuffer, &renderBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);

    VkViewport viewPort{0.0f,
                        0.0f,
                        static_cast<float>(this->resources.extent.width),
                        static_cast<float>(this->resources.extent.height),
                        0.0f,
                        1.0f};
    vkCmdSetViewport(this->commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.extent = this->resources.extent;
    scissor.offset = {0, 0};
    vkCmdSetScissor(this->commandBuffer, 0, 1, &scissor);

    vkCmdDraw(this->commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(this->commandBuffer);

    if (vkEndCommandBuffer(this->commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer!");
};

void Triangle::mainLoop() {
    while (this->appWindow.running) {
        while (SDL_PollEvent(&this->appWindow.event)) {
            if (this->appWindow.event.type == SDL_QUIT)
                this->appWindow.running = false;

            drawFrame();
        }
    }
};

void Triangle::drawFrame() {
    vkWaitForFences(this->device, 1, &this->inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(this->device, 1, &this->inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(this->device, this->swapChain, UINT64_MAX,
                          this->availableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(this->commandBuffer, 0);

    recordCommandBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphore[] = {this->availableSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->commandBuffer;

    VkSemaphore signalSemaphore[] = {this->finishedSemaphore};
    submitInfo.pSignalSemaphores = signalSemaphore;
    submitInfo.signalSemaphoreCount = 1;

    if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo,
                      this->inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pWaitSemaphores = signalSemaphore;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pSwapchains = &this->swapChain;
    presentInfo.swapchainCount = 1;

    vkQueuePresentKHR(this->presentQueue, &presentInfo);
};

void Triangle::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(this->device, &semaphoreInfo, nullptr,
                          &this->availableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(this->device, &semaphoreInfo, nullptr,
                          &this->finishedSemaphore) ||
        vkCreateFence(this->device, &fenceInfo, nullptr, &this->inFlightFence))
        throw std::runtime_error("Failed to create semaphores!");
};

void Triangle::cleanUp() const {
    vkDestroySemaphore(this->device, this->availableSemaphore, nullptr);
    vkDestroySemaphore(this->device, this->finishedSemaphore, nullptr);
    vkDestroyFence(this->device, this->inFlightFence, nullptr);
    vkDestroyCommandPool(this->device, this->commandPool, nullptr);

    for (const auto framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(this->device, framebuffer, nullptr);
    }

    vkDestroyPipeline(this->device, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->device, this->layout, nullptr);
    vkDestroyRenderPass(this->device, this->renderPass, nullptr);
    vkDestroyShaderModule(this->device, this->module, nullptr);

    for (const auto imageView : this->resources.imageViews) {
        vkDestroyImageView(this->device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);
    vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
    vkDestroyDevice(this->device, nullptr);
    vkDestroyInstance(this->instance, nullptr);

    this->appWindow.destroy();
}
