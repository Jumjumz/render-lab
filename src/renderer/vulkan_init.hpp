#ifndef VULKAN_INIT
#define VULKAN_INIT

#pragma once

#include <vulkan/vulkan_raii.hpp>

class VulkanInit {
  public:
    VulkanInit();

    vk::raii::Instance init;
    vk::raii::SurfaceKHR surface;

    vk::raii::Device device;
    vk::raii::PhysicalDevice PhysicalDevice;

  private:
    void createInstance();

    bool checkValidationLayers();

    void pickPhysicalDevice();

    void createLogicalDevice();
};

#endif // !VULKAN_INIT
