#ifndef VULKAN_H
#define VULKAN_H

#pragma once

#include "window/window.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

class Vulkan {
  public:
    Vulkan();

    void run();

  private:
    window appWindow;

    VkInstance instance;
    VkSurfaceKHR surface;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;

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

    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

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

    void initWindow();

    void initVulkan();

    void createInstance();

    bool checkValidationLayers();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void findQueueFamilies();

    void createSurface();

    void surfaceConfig();

    void createSwapChain();

    void createViewImage();

    void createGraphicsPipeline();

    static std::vector<char> readFile(const std::string &fileName);

    [[nodiscard]]
    VkShaderModule createShaderModule(const std::vector<char> &code);

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

#endif // !VULKAN_H
