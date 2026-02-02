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
    vk::raii::PhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

  private:
    void initWindow();

    void createInstance();

    bool checkValidationLayers();

    void physicalDevice();

    void createLogicalDevice();
};

#endif // !VULKAN_INIT
