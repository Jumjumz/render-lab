#ifndef VULKAN_RESOURCES_HPP
#define VULKAN_RESOURCES_HPP

#pragma once

#include <vector>
#include <vulkan/vulkan_raii.hpp>

class VulkanResources {
  public:
    VulkanResources(const vk::raii::PhysicalDevice &physDevice,
                    const vk::raii::Device &device,
                    const vk::raii::Queue &graphicsQueue,
                    const vk::raii::CommandPool &commandPool,
                    const int &MAX_FRAMES_IN_FLIGHT);

    vk::raii::Buffer vertexBuffer = nullptr;
    vk::raii::DeviceMemory vertexMemory = nullptr;

    vk::raii::Buffer indexBuffer = nullptr;
    vk::raii::DeviceMemory indexMemory = nullptr;

    std::vector<vk::raii::Buffer> uniformBuffers;
    std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
    std::vector<void *> uniformBuffersMapped;

  private:
    const vk::raii::PhysicalDevice &physDevice;
    const vk::raii::Device &device;
    const vk::raii::Queue &graphicsQueue;
    const vk::raii::CommandPool &commandPool;

    const int MAX_FRAMES_IN_FLIGHT;

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    uint32_t findMemoryType(uint32_t typeFilter,
                            vk::MemoryPropertyFlags properties);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                      vk::MemoryPropertyFlags properties,
                      vk::raii::Buffer &buffer,
                      vk::raii::DeviceMemory &bufferMemory);

    void copyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer,
                    vk::DeviceSize size);
};

#endif // !VULKAN_RESOURCES_HPP
