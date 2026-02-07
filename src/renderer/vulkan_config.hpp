#ifndef VULKAN_CONFIG_HPP
#define VULKAN_CONFIG_HPP

#pragma once

#include "renderer/vulkan_init.hpp"

#include <string>
#include <vector>

class VulkanConfig {
  public:
    void run();

    VulkanInit vulkanInit;

    vk::raii::Pipeline graphicsPipeline = nullptr;
    vk::raii::PipelineLayout layout = nullptr;

    vk::raii::CommandPool commandPool = nullptr;

    std::vector<vk::raii::CommandBuffer> commandBuffers;
    std::vector<vk::raii::Semaphore> availableSemaphores;
    std::vector<vk::raii::Semaphore> finishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void initVulkan();

    static std::vector<char> readFile(const std::string &fileName);

    [[nodiscard]]
    vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);

    void createGraphicsPipeline();

    void createCommandPool();

    void createCommandBuffers();

    void drawFrame();

    void transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout,
                               vk::AccessFlags2 srcAccessMask,
                               vk::AccessFlags2 dstAccessMask,
                               vk::PipelineStageFlags2 srcStageMask,
                               vk::PipelineStageFlags2 dstStageMask);

    void recordCommandBuffer(uint32_t imageIndex);

    void createSyncObjects();

    void recreateSwapChain();

    void mainLoop();

    void cleanupSwapChain();

    void cleanUp();
};

#endif // !VULKAN_CONFIG_HPP
