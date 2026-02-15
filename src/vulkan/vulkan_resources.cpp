#include "vulkan_resources.hpp"
#include "renderer/vertex.hpp"

VulkanResources::VulkanResources(const vk::raii::PhysicalDevice &physDevice,
                                 const vk::raii::Device &device,
                                 const vk::raii::Queue &graphicsQueue,
                                 const vk::raii::CommandPool &commandPool,
                                 const int &MAX_FRAMES_IN_FLIGHT, Render &shape)
    : physDevice(physDevice), device(device), graphicsQueue(graphicsQueue),
      commandPool(commandPool), MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT),
      shape(shape) {
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
};

void VulkanResources::createVertexBuffer() {
    vk::DeviceSize bufferSize =
        sizeof(this->shape.vertices[0]) * this->shape.vertices.size();

    vk::BufferCreateInfo stagingInfo{};
    stagingInfo.size = bufferSize;
    stagingInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::raii::Buffer stagingBuffer{this->device, stagingInfo, nullptr};

    vk::MemoryRequirements memRequirementsStaging =
        stagingBuffer.getMemoryRequirements();

    vk::MemoryAllocateInfo memAllocateStagingInfo{};
    memAllocateStagingInfo.allocationSize = memRequirementsStaging.size;
    memAllocateStagingInfo.memoryTypeIndex =
        findMemoryType(memRequirementsStaging.memoryTypeBits,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::raii::DeviceMemory stagingBufferMemory{this->device,
                                               memAllocateStagingInfo, nullptr};

    stagingBuffer.bindMemory(stagingBufferMemory, 0);

    void *dataStaging = stagingBufferMemory.mapMemory(0, stagingInfo.size);
    memcpy(dataStaging, this->shape.vertices.data(),
           static_cast<size_t>(stagingInfo.size));

    stagingBufferMemory.unmapMemory();

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer |
                       vk::BufferUsageFlagBits::eTransferDst;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    this->vertexBuffer = vk::raii::Buffer{this->device, bufferInfo, nullptr};

    vk::MemoryRequirements memRequirements =
        this->vertexBuffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memAllocateInfo{};
    memAllocateInfo.allocationSize = memRequirements.size;
    memAllocateInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits,
                       vk::MemoryPropertyFlagBits::eDeviceLocal);

    this->vertexMemory =
        vk::raii::DeviceMemory{this->device, memAllocateInfo, nullptr};

    this->vertexBuffer.bindMemory(*this->vertexMemory, 0);

    copyBuffer(stagingBuffer, this->vertexBuffer, stagingInfo.size);
};

uint32_t VulkanResources::findMemoryType(uint32_t typeFilter,
                                         vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties =
        this->physDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties)
            return i;
    }

    throw std::runtime_error("Failed to find suitable memory type!");
};

void VulkanResources::createIndexBuffer() {
    vk::DeviceSize bufferSize =
        sizeof(this->shape.indices[0]) * this->shape.indices.size();

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, this->shape.indices.data(), static_cast<size_t>(bufferSize));

    stagingBufferMemory.unmapMemory();

    createBuffer(bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst |
                     vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, this->indexBuffer,
                 this->indexMemory);

    copyBuffer(stagingBuffer, this->indexBuffer, bufferSize);
};

void VulkanResources::createUniformBuffers() {
    this->uniformBuffers.clear();
    this->uniformBuffersMemory.clear();
    this->uniformBuffersMapped.clear();

    for (size_t i = 0; i < (size_t)this->MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        vk::raii::Buffer buffer({});
        vk::raii::DeviceMemory bufferMem({});

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent,
                     buffer, bufferMem);

        this->uniformBuffers.emplace_back(std::move(buffer));
        this->uniformBuffersMemory.emplace_back(std::move(bufferMem));
        this->uniformBuffersMapped.emplace_back(
            this->uniformBuffersMemory[i].mapMemory(0, bufferSize));
    }
};

void VulkanResources::createBuffer(vk::DeviceSize size,
                                   vk::BufferUsageFlags usage,
                                   vk::MemoryPropertyFlags properties,
                                   vk::raii::Buffer &buffer,
                                   vk::raii::DeviceMemory &bufferMemory) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = vk::raii::Buffer{this->device, bufferInfo, nullptr};

    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

    vk::MemoryAllocateInfo allocateInfo{};
    allocateInfo.allocationSize = memRequirements.size;
    allocateInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = vk::raii::DeviceMemory{this->device, allocateInfo, nullptr};

    buffer.bindMemory(*bufferMemory, 0);
};

void VulkanResources::copyBuffer(vk::raii::Buffer &srcBuffer,
                                 vk::raii::Buffer &dstBuffer,
                                 vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = this->commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::raii::CommandBuffer commandCopyBuffer =
        std::move(this->device.allocateCommandBuffers(allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    commandCopyBuffer.begin(beginInfo);
    commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer,
                                 vk::BufferCopy{0, 0, size});

    commandCopyBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*commandCopyBuffer;

    this->graphicsQueue.submit(submitInfo, nullptr);
    this->graphicsQueue.waitIdle();
};
