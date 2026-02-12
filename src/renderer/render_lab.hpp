#ifndef RENDER_LAB_HPP
#define RENDER_LAB_HPP

#pragma once

#include "vulkan/vulkan_commands.hpp"
#include "vulkan/vulkan_context.hpp"
#include "vulkan/vulkan_graphics.hpp"
#include "vulkan/vulkan_resources.hpp"
#include "vulkan/vulkan_swapchain.hpp"

#include <cstdint>

class RenderLab {
  public:
    RenderLab();

    void run();

    void loop();

    void cleanSwapchain();

    void cleanUp();

  private:
    VulkanContext ctx;
    VulkanSwapchain swapchain;
    VulkanResources resources;
    VulkanGraphics graphics;
    VulkanCommands commands;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;
};

#endif // !RENDER_LAB_HPP
