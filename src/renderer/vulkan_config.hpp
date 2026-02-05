#ifndef VULKAN_CONFIG_HPP
#define VULKAN_CONFIG_HPP

#pragma once

#include "renderer/vulkan_init.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

class VulkanConfig {
  public:
    VulkanConfig();

    void run();

  private:
    VulkanInit vulkanInit;

    VkShaderModule module;
    VkRenderPass renderPass;

    VkPipeline graphicsPipeline;
    VkPipelineLayout layout;

    VkCommandPool commandPool;

    VkBuffer vertexBuffer;

    std::vector<VkSemaphore> availableSemaphores;
    std::vector<VkSemaphore> finishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkCommandBuffer> commandBuffers;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void createGraphicsPipeline();

    static std::vector<char> readFile(const std::string &fileName);

    [[nodiscard]]
    VkShaderModule createShaderModule(const std::vector<char> &code);

    void initVulkan();

    void createRenderPass();

    void createFrameBuffers();

    void createCommandPool();

    void createCommandBuffers();

    void mainLoop();

    void drawFrame();

    void createSyncObjects();

    void recordCommandBuffer(VkCommandBuffer &commandBuffer,
                             uint32_t &imageIndex);

    void recreateSwapChain();

    void cleanupSwapChain();

    void cleanUp();
};

#endif // !VULKAN_CONFIG_HPP
