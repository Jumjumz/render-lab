#include "vulkan_resources.hpp"
#include "renderer/vertex.hpp"

VulkanResources::VulkanResources(const vk::raii::PhysicalDevice &physDevice,
                                 const vk::raii::Device &device,
                                 const vk::raii::Queue &graphicsQueue,
                                 const vk::raii::CommandPool &commandPool,
                                 const vk::Extent2D &extent,
                                 const vk::Format &depthFormat,
                                 const int &MAX_FRAMES_IN_FLIGHT,
                                 const Render &shape)
    : physDevice(physDevice), device(device), graphicsQueue(graphicsQueue),
      commandPool(commandPool), extent(extent), depthFormat(depthFormat),
      MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT), shape(shape) {
    createDepthResources();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
};

void VulkanResources::createDepthResources() {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = this->depthFormat;
    imageInfo.extent.width = this->extent.width;
    imageInfo.extent.height = this->extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

    this->depthImage = vk::raii::Image{this->device, imageInfo, nullptr};

    vk::MemoryRequirements memReqs = this->depthImage.getMemoryRequirements();

    vk::MemoryAllocateInfo memInfo{};
    memInfo.allocationSize = memReqs.size;
    memInfo.memoryTypeIndex = findMemoryType(
        memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    this->depthImageMemory =
        vk::raii::DeviceMemory{this->device, memInfo, nullptr};

    this->depthImage.bindMemory(this->depthImageMemory, 0);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.image = this->depthImage;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = this->depthFormat;
    viewInfo.subresourceRange = {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1};

    this->depthImageView = vk::raii::ImageView{this->device, viewInfo, nullptr};
};

void VulkanResources::createVertexBuffer() {
    if (this->shape.renderData.render == true) {
        this->vertexMemory = nullptr;
        this->vertexBuffer = nullptr;
    }

    vk::DeviceSize bufferSize = sizeof(this->shape.renderData.vertices[0]) *
                                this->shape.renderData.vertices.size();

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
    memcpy(dataStaging, this->shape.renderData.vertices.data(),
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
    if (this->shape.renderData.render == true)
        this->indexBuffer = nullptr;

    vk::DeviceSize bufferSize = sizeof(this->shape.renderData.indices[0]) *
                                this->shape.renderData.indices.size();

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, this->shape.renderData.indices.data(),
           static_cast<size_t>(bufferSize));

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

    vk::MemoryAllocateInfo memAllocInfo{};
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = vk::raii::DeviceMemory{this->device, memAllocInfo, nullptr};

    buffer.bindMemory(*bufferMemory, 0);
};

void VulkanResources::copyBuffer(vk::raii::Buffer &srcBuffer,
                                 vk::raii::Buffer &dstBuffer,
                                 vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.commandPool = this->commandPool;
    cmdAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdAllocInfo.commandBufferCount = 1;

    vk::raii::CommandBuffer cmdBuffer =
        std::move(this->device.allocateCommandBuffers(cmdAllocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmdBuffer.begin(beginInfo);
    cmdBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy{0, 0, size});

    cmdBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*cmdBuffer;

    this->graphicsQueue.submit(submitInfo, nullptr);
    this->graphicsQueue.waitIdle();
};
