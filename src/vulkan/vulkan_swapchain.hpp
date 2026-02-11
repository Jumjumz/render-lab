#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#pragma once

#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

class VulkanSwapchain {
  public:
    VulkanSwapchain(const vk::raii::SurfaceKHR &surface,
                    const vk::raii::Device &device,
                    const vk::SurfaceCapabilitiesKHR &capabilities,
                    const vk::SurfaceFormatKHR &format,
                    const vk::PresentModeKHR &presentMode,
                    const vk::Extent2D &extent, const int &graphicsFamily,
                    const int &presentFamily, const uint32_t &imageCount);

    // swapchain
    vk::raii::SwapchainKHR swapchain = nullptr;

    struct SwapchainResources {
        std::vector<vk::Image> images;
        std::vector<vk::raii::ImageView> imageViews;

        vk::Format imageFormat;
        vk::Extent2D extent;
    } resources;

    void recreateSwapChain();

    void cleanupSwapchain();

  private:
    const vk::raii::SurfaceKHR &surface;

    const vk::raii::Device &device;

    const vk::SurfaceCapabilitiesKHR capabilities;
    const vk::SurfaceFormatKHR format;
    const vk::PresentModeKHR presentMode;
    const vk::Extent2D extent;

    const int graphicsFamily;
    const int presentFamily;

    const uint32_t imageCount;

    void createSwapchain();

    void createImageView();
};

#endif // !VULKAN_SWAPCHAIN_HPP
