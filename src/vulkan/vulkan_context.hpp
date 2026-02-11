#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#pragma once

#include <SDL_video.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

class VulkanContext {
  public:
    VulkanContext(SDL_Window *window);

    // core
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;

    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::Queue presentQueue = nullptr;

    vk::raii::CommandPool commandPool = nullptr;

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

  private:
    SDL_Window *window;

    void createInstance();

    void pickPhysicalDevice();

    void findQueueFamilies();

    void createLogicalDevice();

    void createSurface();

    void surfaceConfig();

    void createCommandPool();
};

#endif // !VULKAN_CONTEXT_HPP
