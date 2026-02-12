#ifndef VULKAN_GRAPHICS_HPP
#define VULKAN_GRAPHICS_HPP

#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

class VulkanGraphics {
  public:
    VulkanGraphics(const vk::raii::Device &device,
                   const vk::Format &imageFormat, const int &graphicsFamily);

    // command pool
    vk::raii::CommandPool commandPool = nullptr;

    // shader module
    vk::raii::ShaderModule shaderModule = nullptr;

    // pipeline
    vk::raii::Pipeline graphicsPipeline = nullptr;
    vk::raii::PipelineLayout layout = nullptr;

    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;

  private:
    const vk::raii::Device &device;

    const vk::Format &imageFormat;

    const int graphicsFamily;

    void createDescriptorSetLayout();

    void createGraphicsPipeline();

    void createCommandPool();

    static std::vector<char> readFile(const std::string &fileName);

    [[nodiscard]]
    vk::raii::ShaderModule createShaderModule(const std::vector<char> &code) const;
};

#endif // !VULKAN_GRAPHICS_HPP
