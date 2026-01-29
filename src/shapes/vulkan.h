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
    VkCommandBuffer commandBuffer;

    VkBuffer vertexBuffer;

    VkSemaphore availableSemaphore;
    VkSemaphore finishedSemaphore;
    VkFence inFlightFence;

    std::vector<VkFramebuffer> framebuffers;

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

    void checkValidationLayers();

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

    void createCommandBuffer();

    void recordCommandBuffer(uint32_t &imageIndex);

    void mainLoop();

    void drawFrame();

    void createSyncObjects();

    void cleanUp() const;
};

#endif // !VULKAN_H
