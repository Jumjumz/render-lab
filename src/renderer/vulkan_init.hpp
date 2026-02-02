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

    vk::raii::Instance instance = VK_NULL_HANDLE;
    vk::raii::SurfaceKHR surface = VK_NULL_HANDLE;

    vk::raii::Device device = VK_NULL_HANDLE;
    vk::raii::PhysicalDevice physicalDevice = VK_NULL_HANDLE;

    vk::raii::Queue graphicsQueue = VK_NULL_HANDLE;
    vk::raii::Queue presentQueue = VK_NULL_HANDLE;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const {
            return graphicsFamily >= 0 && presentFamily >= 0;
        };
    } familyIndices;

  private:
    void initWindow();

    void createInstance();

    void pickPhysicalDevice();

    void findQueueFamilies();

    void createLogicalDevice();
};

#endif // !VULKAN_INIT
