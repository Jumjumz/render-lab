#include "vulkan_commands.hpp"
#include "renderer/vertex.hpp"
#include <cstddef>

VulkanCommands::VulkanCommands(
    const vk::raii::Device &device, const std::vector<vk::Image> &images,
    const vk::raii::CommandPool &commandPool,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const std::vector<vk::raii::Buffer> &uniformBuffers,
    const int &MAX_FRAMES_IN_FLIGHT)
    : device(device), images(images), commandPool(commandPool),
      descriptorSetLayout(descriptorSetLayout), uniformBuffers(uniformBuffers),
      MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT) {
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
};

void VulkanCommands::createDescriptorPool() {
    vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer,
                                    VulkanCommands::MAX_FRAMES_IN_FLIGHT);

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    poolInfo.maxSets = VulkanCommands::MAX_FRAMES_IN_FLIGHT;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    this->descriptorPool =
        vk::raii::DescriptorPool{this->device, poolInfo, nullptr};
};

void VulkanCommands::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(this->MAX_FRAMES_IN_FLIGHT,
                                                 *this->descriptorSetLayout);

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = this->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    this->descriptorSets.clear();
    this->descriptorSets = this->device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < (size_t)this->MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = this->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = this->descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.pBufferInfo = &bufferInfo;

        this->device.updateDescriptorSets(descriptorWrite, {});
    }
};

void VulkanCommands::createCommandBuffers() {
    this->commandBuffers.clear();

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount = this->MAX_FRAMES_IN_FLIGHT;

    this->commandBuffers = vk::raii::CommandBuffers{
        this->device,
        allocInfo,
    };
};

void VulkanCommands::createSyncObjects() {
    assert(this->availableSemaphores.empty() &&
           this->finishedSemaphores.empty() && this->inFlightFences.empty());

    for (size_t i = 0; i < this->images.size(); i++) {
        this->finishedSemaphores.emplace_back(this->device,
                                              vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < (size_t)this->MAX_FRAMES_IN_FLIGHT; i++) {
        this->availableSemaphores.emplace_back(this->device,
                                               vk::SemaphoreCreateInfo());

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        this->inFlightFences.emplace_back(this->device, fenceInfo);
    }
};
