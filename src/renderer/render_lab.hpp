#ifndef RENDER_LAB_HPP
#define RENDER_LAB_HPP

#pragma once

#include "renderer/render.hpp"
#include "utils/resolution.hpp"
#include "vulkan/vulkan_commands.hpp"
#include "vulkan/vulkan_context.hpp"
#include "vulkan/vulkan_graphics.hpp"
#include "vulkan/vulkan_resources.hpp"
#include "vulkan/vulkan_swapchain.hpp"
#include "window/window.h"
#include <cstdint>

class RenderLab {
  public:
    RenderLab(Resolution &res, Aspect &aspect, const Render &shape);

    void run();

  private:
    uint width;
    float aspectRatio;

    const Render &shape;

    Window window{width, aspectRatio};

    VulkanContext ctx{this->window.sdl_window};

    VulkanSwapchain swapchain{this->ctx.surface,
                              this->ctx.physicalDevice,
                              this->ctx.device,
                              this->ctx.config.capabilities,
                              this->ctx.config.chosenFormat,
                              this->ctx.config.chosenPresentMode,
                              this->ctx.config.chosenExtent,
                              this->ctx.familyIndices.graphicsFamily,
                              this->ctx.familyIndices.presentFamily,
                              this->ctx.config.imageCount};

    VulkanGraphics graphics{this->ctx.physicalDevice, this->ctx.device,
                            this->swapchain.resources.imageFormat,
                            this->swapchain.depthFormat,
                            this->ctx.familyIndices.graphicsFamily};

    VulkanResources resources{
        this->ctx.physicalDevice,        this->ctx.device,
        this->ctx.graphicsQueue,         this->graphics.commandPool,
        this->ctx.config.chosenExtent,   this->swapchain.depthFormat,
        RenderLab::MAX_FRAMES_IN_FLIGHT, shape};

    VulkanCommands commands{this->ctx.device,
                            this->swapchain.resources.images,
                            this->graphics.commandPool,
                            this->graphics.descriptorSetLayout,
                            this->resources.uniformBuffers,
                            RenderLab::MAX_FRAMES_IN_FLIGHT};

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void loop();

    void setup();

    void drawFrame();

    void recordCommandBuffer(uint32_t imageIndex);

    void transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout,
                               vk::AccessFlags2 srcAccessMask,
                               vk::AccessFlags2 dstAccessMask,
                               vk::PipelineStageFlags2 srcStageMask,
                               vk::PipelineStageFlags2 dstStageMask,
                               vk::ImageAspectFlags imageAspectFlags);

    void recreateSwapchain();

    void cleanSwapchain();

    void cleanUp();
};

#endif // !RENDER_LAB_HPP
