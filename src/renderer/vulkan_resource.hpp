#ifndef VULKAN_RESOURCE
#define VULKAN_RESOURCE

#pragma once

#include "window/window.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

class VulkanResource {
  public:
    VulkanResource();

    window appWindow;

    // c vulkan
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;

    // core
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;

    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::Queue presentQueue = nullptr;

    // swapchain
    vk::raii::SwapchainKHR swapChain = nullptr;

    struct SwapchainResources {
        std::vector<vk::Image> images;
        std::vector<vk::raii::ImageView> imageViews;

        vk::Format imageFormat;
        vk::Extent2D extent;
    } resources;

    // pipeline
    vk::raii::Pipeline graphicsPipeline = nullptr;
    vk::raii::PipelineLayout layout = nullptr;

    // commands
    vk::raii::CommandPool commandPool = nullptr;

    std::vector<vk::raii::CommandBuffer> commandBuffers;
    std::vector<vk::raii::Semaphore> availableSemaphores;
    std::vector<vk::raii::Semaphore> finishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    void run();

  private:
    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const {
            return graphicsFamily >= 0 && presentFamily >= 0;
        };
    } familyIndices;

    // surface
    struct SurfaceConfig {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        vk::SurfaceFormatKHR chosenFormat;
        vk::PresentModeKHR chosenPresentMode;
        vk::Extent2D chosenExtent;

        uint32_t imageCount;
    } config;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void initWindow();

    void createInstance();

    void pickPhysicalDevice();

    void findQueueFamilies();

    void createLogicalDevice();

    void createSurface();

    void surfaceConfig();

    void createSwapChain();

    void createViewImage();

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

#endif // !VULKAN_RESOURCE
