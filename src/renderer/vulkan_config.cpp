#include "vulkan_config.hpp"

#include "ndebug.h"
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <SDL_vulkan.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

VulkanConfig::VulkanConfig() {};

void VulkanConfig::run() {
    initVulkan();
    mainLoop();
    cleanUp();
};

void VulkanConfig::initVulkan() {
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
};

void VulkanConfig::createGraphicsPipeline() {
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

    if (vkCreatePipelineLayout(this->vulkanInit.device, &layoutInfo, nullptr,
                               &this->layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout!");

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats =
        &this->vulkanInit.swapchainResources.imageFormat;

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

    if (vkCreateGraphicsPipelines(this->vulkanInit.device, VK_NULL_HANDLE, 1,
                                  &graphicsInfo, nullptr,
                                  &this->graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create a graphics pipeline!");
};

std::vector<char> VulkanConfig::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    return buffer;
};

[[nodiscard]]
VkShaderModule VulkanConfig::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(this->vulkanInit.device, &shaderInfo, nullptr,
                             &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module!");

    this->module = shaderModule; // wip

    return shaderModule;
};

void VulkanConfig::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = this->vulkanInit.swapchainResources.imageFormat;
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

    if (vkCreateRenderPass(this->vulkanInit.device, &renderInfo, nullptr,
                           &this->renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass!");
};

void VulkanConfig::createFrameBuffers() {
    framebuffers.resize(this->vulkanInit.swapchainResources.imageViews.size());

    for (size_t i = 0;
         i < this->vulkanInit.swapchainResources.imageViews.size(); i++) {
        VkImageView attachments[] = {
            this->vulkanInit.swapchainResources.imageViews[i]};

        VkFramebufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        bufferInfo.pAttachments = attachments;
        bufferInfo.renderPass = this->renderPass;
        bufferInfo.width = this->vulkanInit.swapchainResources.extent.width;
        bufferInfo.height = this->vulkanInit.swapchainResources.extent.height;
        bufferInfo.attachmentCount = 1;
        bufferInfo.layers = 1;

        if (vkCreateFramebuffer(this->vulkanInit.device, &bufferInfo, nullptr,
                                &this->framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create a framebuffers!");
    }
};

void VulkanConfig::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex =
        static_cast<uint32_t>(this->vulkanInit.familyIndices.graphicsFamily);

    if (vkCreateCommandPool(this->vulkanInit.device, &poolInfo, nullptr,
                            &this->commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool!");
};

void VulkanConfig::createCommandBuffers() {
    this->commandBuffers.resize(
        static_cast<size_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT));

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(this->commandBuffers.size());

    if (vkAllocateCommandBuffers(this->vulkanInit.device, &allocInfo,
                                 this->commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer");
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

    vkDeviceWaitIdle(this->vulkanInit.device);
};

void VulkanConfig::drawFrame() {
    vkWaitForFences(this->vulkanInit.device, 1,
                    &this->inFlightFences[this->currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        this->vulkanInit.device, this->vulkanInit.swapChain, UINT64_MAX,
        this->availableSemaphores[this->currentFrame], VK_NULL_HANDLE,
        &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(this->vulkanInit.device, 1,
                  &this->inFlightFences[this->currentFrame]);

    vkResetCommandBuffer(this->commandBuffers[this->currentFrame], 0);

    recordCommandBuffer(this->commandBuffers[this->currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = &this->commandBuffers[this->currentFrame];

    VkSemaphore waitSemaphore[] = {this->availableSemaphores[this->currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphore[] = {this->finishedSemaphores[this->currentFrame]};
    submitInfo.pSignalSemaphores = signalSemaphore;
    submitInfo.signalSemaphoreCount = 1;

    if (vkQueueSubmit(this->vulkanInit.graphicsQueue, 1, &submitInfo,
                      this->inFlightFences[this->currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphore;
    presentInfo.pImageIndices = &imageIndex;

    VkSwapchainKHR swapChains[] = {this->vulkanInit.swapChain};
    presentInfo.pSwapchains = swapChains;
    presentInfo.swapchainCount = 1;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(this->vulkanInit.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        this->framebufferResized) {
        this->framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    this->currentFrame =
        (this->currentFrame + 1) % VulkanConfig::MAX_FRAMES_IN_FLIGHT;
};

void VulkanConfig::recordCommandBuffer(VkCommandBuffer &commandBuffer,
                                       uint32_t &imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderBeginInfo{};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = this->renderPass;
    renderBeginInfo.framebuffer = this->framebuffers[imageIndex];
    renderBeginInfo.renderArea.offset = {0, 0};
    renderBeginInfo.renderArea.extent =
        this->vulkanInit.swapchainResources.extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);

    VkViewport viewPort{
        0.0f,
        0.0f,
        static_cast<float>(this->vulkanInit.swapchainResources.extent.width),
        static_cast<float>(this->vulkanInit.swapchainResources.extent.height),
        0.0f,
        1.0f};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.extent = this->vulkanInit.swapchainResources.extent;
    scissor.offset = {0, 0};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer!");
};

void VulkanConfig::createSyncObjects() {
    this->availableSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
    this->finishedSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
    this->inFlightFences.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < size_t(VulkanConfig::MAX_FRAMES_IN_FLIGHT); i++) {
        if (vkCreateSemaphore(this->vulkanInit.device, &semaphoreInfo, nullptr,
                              &this->availableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(this->vulkanInit.device, &semaphoreInfo, nullptr,
                              &this->finishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(this->vulkanInit.device, &fenceInfo, nullptr,
                          &this->inFlightFences[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create semaphores!");
    }
};

void VulkanConfig::recreateSwapChain() {
    vkDeviceWaitIdle(this->vulkanInit.device);

    cleanupSwapChain();

    createSwapChain();
    createViewImage();
    createFrameBuffers();
};

void VulkanConfig::cleanupSwapChain() {
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(this->vulkanInit.device, framebuffer, nullptr);
    }

    for (auto imageView : vulkanInit.swapchainResources.imageViews) {
        vkDestroyImageView(this->vulkanInit.device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(this->vulkanInit.device, this->vulkanInit.swapChain,
                          nullptr);
};

void VulkanConfig::cleanUp() {
    cleanupSwapChain();

    for (size_t i = 0; i < size_t(VulkanConfig::MAX_FRAMES_IN_FLIGHT); i++) {
        vkDestroySemaphore(this->vulkanInit.device,
                           this->availableSemaphores[i], nullptr);
        vkDestroySemaphore(this->vulkanInit.device, this->finishedSemaphores[i],
                           nullptr);
        vkDestroyFence(this->vulkanInit.device, this->inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(this->vulkanInit.device, this->commandPool, nullptr);

    vkDestroyPipeline(this->vulkanInit.device, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->vulkanInit.device, this->layout, nullptr);
    vkDestroyRenderPass(this->vulkanInit.device, this->renderPass, nullptr);
    vkDestroyShaderModule(this->vulkanInit.device, this->module, nullptr);

    this->vulkanInit.appWindow.destroy();
};
