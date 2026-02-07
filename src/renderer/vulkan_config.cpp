#include "vulkan_config.hpp"

#include <fstream>

void VulkanConfig::run() {
    initVulkan();
    mainLoop();
    cleanUp();
};

void VulkanConfig::initVulkan() {
    createGraphicsPipeline();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
};

[[nodiscard]]
vk::raii::ShaderModule
VulkanConfig::createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo shaderInfo{};
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    vk::raii::ShaderModule shaderModule{this->vulkanInit.device, shaderInfo};

    return shaderModule;
};

std::vector<char> VulkanConfig::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
};

void VulkanConfig::createGraphicsPipeline() {
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

    this->layout =
        vk::raii::PipelineLayout{this->vulkanInit.device, layoutInfo, nullptr};

    vk::PipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats =
        &this->vulkanInit.resources.imageFormat;

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
    pipelineInfo.renderPass = VK_NULL_HANDLE;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    this->graphicsPipeline = vk::raii::Pipeline{
        this->vulkanInit.device, VK_NULL_HANDLE, pipelineInfo, VK_NULL_HANDLE};
};

void VulkanConfig::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex =
        static_cast<uint32_t>(this->vulkanInit.familyIndices.graphicsFamily);

    this->commandPool =
        vk::raii::CommandPool{this->vulkanInit.device, poolInfo, VK_NULL_HANDLE};
};

void VulkanConfig::createCommandBuffers() {
    this->commandBuffers.clear();

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount = VulkanConfig::MAX_FRAMES_IN_FLIGHT;

    this->commandBuffers =
        vk::raii::CommandBuffers{this->vulkanInit.device, allocInfo};
};

void VulkanConfig::drawFrame() {
    auto fenceResult = this->vulkanInit.device.waitForFences(
        *this->inFlightFences[this->currentFrame], vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence!");
    }

    auto [result, imageIndex] = this->vulkanInit.swapChain.acquireNextImage(
        UINT64_MAX, *this->availableSemaphores[this->currentFrame],
        VK_NULL_HANDLE);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess &&
               result != vk::Result::eSuboptimalKHR) {
        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    this->vulkanInit.device.resetFences(*this->inFlightFences[this->currentFrame]);

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

    this->vulkanInit.graphicsQueue.submit(
        submitInfo, *this->inFlightFences[this->currentFrame]);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &*this->finishedSemaphores[this->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &*this->vulkanInit.swapChain;
    presentInfo.pImageIndices = &imageIndex;

    result = this->vulkanInit.presentQueue.presentKHR(presentInfo);

    if ((result == vk::Result::eSuboptimalKHR) ||
        (result == vk::Result::eErrorOutOfDateKHR) || framebufferResized) {
        this->framebufferResized = false;
        recreateSwapChain();
    } else {
        assert(result == vk::Result::eSuccess);
    }

    this->currentFrame =
        (this->currentFrame + 1) % VulkanConfig::MAX_FRAMES_IN_FLIGHT;
};

void VulkanConfig::transitionImageLayout(uint32_t imageIndex,
                                         vk::ImageLayout oldLayout,
                                         vk::ImageLayout newLayout,
                                         vk::AccessFlags2 srcAccessMask,
                                         vk::AccessFlags2 dstAccessMask,
                                         vk::PipelineStageFlags2 srcStageMask,
                                         vk::PipelineStageFlags2 dstStageMask) {
    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcStageMask = srcStageMask;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstStageMask = dstStageMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.image = this->vulkanInit.resources.images[imageIndex];
    barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    this->commandBuffers[this->currentFrame].pipelineBarrier2(dependencyInfo);
};

void VulkanConfig::recordCommandBuffer(uint32_t imageIndex) {
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
    attachmentInfo.imageView = this->vulkanInit.resources.imageViews[imageIndex];
    attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentInfo.clearValue = clearColor;

    vk::Offset2D offset = {0, 0};

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea.offset = offset;
    renderingInfo.renderArea.extent = this->vulkanInit.resources.extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &attachmentInfo;

    cmd.beginRendering(renderingInfo);

    // render commands
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, this->graphicsPipeline);
    cmd.setViewport(
        0, vk::Viewport{
               0.0f, 0.0f,
               static_cast<float>(this->vulkanInit.resources.extent.width),
               static_cast<float>(this->vulkanInit.resources.extent.height),
               0.0f, 1.0f});

    cmd.setScissor(
        0, vk::Rect2D{vk::Offset2D{0, 0}, this->vulkanInit.resources.extent});

    cmd.draw(3, 1, 0, 0);

    cmd.endRendering();

    transitionImageLayout(imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ImageLayout::ePresentSrcKHR,
                          vk::AccessFlagBits2::eColorAttachmentWrite, {},
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eBottomOfPipe);

    cmd.end();
};

void VulkanConfig::createSyncObjects() {
    assert(this->availableSemaphores.empty() &&
           this->finishedSemaphores.empty() && this->inFlightFences.empty());

    for (size_t i = 0; i < this->vulkanInit.resources.images.size(); i++) {
        this->finishedSemaphores.emplace_back(this->vulkanInit.device,
                                              vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++) {
        this->availableSemaphores.emplace_back(this->vulkanInit.device,
                                               vk::SemaphoreCreateInfo());

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        this->inFlightFences.emplace_back(this->vulkanInit.device, fenceInfo);
    }
};

void VulkanConfig::mainLoop() {
    while (this->vulkanInit.appWindow.running) {
        while (SDL_PollEvent(&this->vulkanInit.appWindow.event)) {
            if (this->vulkanInit.appWindow.event.type == SDL_QUIT)
                this->vulkanInit.appWindow.running = false;

            if (this->vulkanInit.appWindow.event.type == SDL_WINDOWEVENT_RESIZED)
                this->framebufferResized = true;
        }

        drawFrame();
    }

    this->vulkanInit.device.waitIdle();
};

void VulkanConfig::recreateSwapChain() {
    this->vulkanInit.device.waitIdle();

    cleanupSwapChain();

    this->vulkanInit.createSwapChain();
    this->vulkanInit.createViewImage();
};

void VulkanConfig::cleanupSwapChain() {
    this->vulkanInit.resources.imageViews.clear();
    this->vulkanInit.swapChain = VK_NULL_HANDLE;
};

void VulkanConfig::cleanUp() {
    cleanupSwapChain();

    this->vulkanInit.clearVk();
    this->vulkanInit.appWindow.destroy();
};
