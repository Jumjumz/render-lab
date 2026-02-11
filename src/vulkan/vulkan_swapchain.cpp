#include "vulkan_swapchain.hpp"

VulkanSwapchain::VulkanSwapchain(
    const vk::raii::SurfaceKHR &surface, const vk::raii::Device &device,
    const vk::SurfaceCapabilitiesKHR &capabilities,
    const vk::SurfaceFormatKHR &format, const vk::PresentModeKHR &presentMode,
    const vk::Extent2D &extent, const int &graphicsFamily,
    const int &presentFamily, const uint32_t &imageCount)
    : surface(surface), device(device), capabilities(capabilities),
      format(format), presentMode(presentMode), extent(extent),
      graphicsFamily(graphicsFamily), presentFamily(presentFamily),
      imageCount(imageCount) {
    createSwapchain();
    createImageView();
};

void VulkanSwapchain::createSwapchain() {
    vk::SwapchainCreateInfoKHR chainInfo{};
    chainInfo.flags = vk::SwapchainCreateFlagsKHR();
    chainInfo.surface = this->surface;
    chainInfo.minImageCount = this->imageCount;
    chainInfo.imageFormat = this->format.format;
    chainInfo.imageColorSpace = this->format.colorSpace;
    chainInfo.imageExtent = this->extent;
    chainInfo.imageArrayLayers = 1;
    chainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queueFamilyIndeces[] = {static_cast<uint32_t>(this->graphicsFamily),
                                     static_cast<uint32_t>(this->presentFamily)};

    if (this->graphicsFamily != this->presentFamily) {
        chainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        chainInfo.queueFamilyIndexCount = 2;
        chainInfo.pQueueFamilyIndices = queueFamilyIndeces;
    } else {
        chainInfo.imageSharingMode = vk::SharingMode::eExclusive;
        chainInfo.queueFamilyIndexCount = 0;
        chainInfo.pQueueFamilyIndices = nullptr;
    }

    chainInfo.preTransform = this->capabilities.currentTransform;
    chainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    chainInfo.presentMode = this->presentMode;
    chainInfo.clipped = vk::True;
    chainInfo.oldSwapchain = nullptr;

    this->swapchain = vk::raii::SwapchainKHR{this->device, chainInfo};
    this->resources.images = this->swapchain.getImages();
};

void VulkanSwapchain::createImageView() {
    this->resources.imageViews.clear();

    for (auto const &image : this->resources.images) {
        vk::ImageViewCreateInfo imageInfo{};
        imageInfo.image = image;
        imageInfo.format = this->format.format;
        imageInfo.viewType = vk::ImageViewType::e2D;
        imageInfo.components.r = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imageInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imageInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                      1};

        this->resources.imageViews.emplace_back(this->device, imageInfo);
    }

    this->resources.extent = this->extent;
    this->resources.imageFormat = this->format.format;
};

void VulkanSwapchain::recreateSwapChain() {
    this->device.waitIdle();

    cleanupSwapchain();

    createSwapchain();
    createImageView();
};

void VulkanSwapchain::cleanupSwapchain() {
    this->resources.imageViews.clear();
    this->swapchain = nullptr;
};
