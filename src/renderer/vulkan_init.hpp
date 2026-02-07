#ifndef VULKAN_INIT
#define VULKAN_INIT

#pragma once

#include "window/window.h"

#include <vulkan/vulkan_raii.hpp>

class VulkanInit {
  public:
    VulkanInit();

    window appWindow;

    // c vulkan
    VkInstance vkInstance;
    VkSurfaceKHR vkSurface;

    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;

    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::Queue presentQueue = nullptr;

    vk::raii::SwapchainKHR swapChain = nullptr;

    struct SwapchainResources {
        std::vector<vk::Image> images;
        std::vector<vk::raii::ImageView> imageViews;

        vk::Format imageFormat;
        vk::Extent2D extent;
    } resources;

    struct SurfaceConfig {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        vk::SurfaceFormatKHR chosenFormat;
        vk::PresentModeKHR chosenPresentMode;
        vk::Extent2D chosenExtent;

        uint32_t imageCount;
    } config;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const {
            return graphicsFamily >= 0 && presentFamily >= 0;
        };
    } familyIndices;

    void createSwapChain();

    void createViewImage();

    void clearVk() const;

  private:
    void initWindow();

    void createInstance();

    void pickPhysicalDevice();

    void findQueueFamilies();

    void createLogicalDevice();

    void createSurface();

    void surfaceConfig();
};

#endif // !VULKAN_INIT
