#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#pragma once

#include "window/window.h"
#include <vector>
#include <vulkan/vulkan.hpp>

class Triangle {
  public:
    Triangle();

    void run();

  private:
    window appWindow;

    VkInstance instance;
    VkSurfaceKHR surface;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSwapchainKHR swapChain;

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

    void pickPhysicalDevice();

    void createLogicalDevice();

    void findQueueFamilies();

    void createSurface();

    void surfaceConfig();

    void createSwapChain();

    void createViewImage();

    void createGraphicsPipeline();

    void mainLoop();

    void cleanUp() const;
};

#endif // !TRIANGLE_HPP
