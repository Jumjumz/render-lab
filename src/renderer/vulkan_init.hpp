#ifndef VULKAN_INIT
#define VULKAN_INIT

#pragma once

#include "window/window.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

class VulkanInit {
  public:
    VulkanInit();

    window appWindow;

    vk::raii::Context context;
    vk::raii::Instance instance = VK_NULL_HANDLE;
    vk::raii::SurfaceKHR surface = VK_NULL_HANDLE;

    vk::raii::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vk::raii::Device device = VK_NULL_HANDLE;

    vk::raii::Queue graphicsQueue = VK_NULL_HANDLE;
    vk::raii::Queue presentQueue = VK_NULL_HANDLE;

    vk::raii::SwapchainKHR swapChain = VK_NULL_HANDLE;

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
