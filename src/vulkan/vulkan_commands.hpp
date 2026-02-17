#ifndef VULKAN_COMMANDS_HPP
#define VULKAN_COMMANDS_HPP

#pragma once

#include <vector>
#include <vulkan/vulkan_raii.hpp>

class VulkanCommands {
  public:
    VulkanCommands(const vk::raii::Device &device,
                   const std::vector<vk::Image> &images,
                   const vk::raii::CommandPool &commandPool,
                   const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                   const std::vector<vk::raii::Buffer> &uniformBuffers,
                   const int &MAX_FRAMES_IN_FLIGHT);

    std::vector<vk::raii::CommandBuffer> commandBuffers;
    std::vector<vk::raii::Semaphore> availableSemaphores;
    std::vector<vk::raii::Semaphore> finishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    vk::raii::DescriptorPool descriptorPool = nullptr;

    std::vector<vk::raii::DescriptorSet> descriptorSets;

  private:
    const vk::raii::Device &device;

    const std::vector<vk::Image> &images;

    const vk::raii::CommandPool &commandPool;

    const vk::raii::DescriptorSetLayout &descriptorSetLayout;

    const std::vector<vk::raii::Buffer> &uniformBuffers;

    const int &MAX_FRAMES_IN_FLIGHT;

    void createDescriptorPool();

    void createDescriptorSets();

    void createCommandBuffers();

    void createSyncObjects();
};

#endif // !VULKAN_COMMANDS_HPP
