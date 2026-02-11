#include "vulkan_graphics.hpp"
#include "renderer/vertex.hpp"
#include <fstream>

VulkanGraphics::VulkanGraphics(const vk::raii::Device &device,
                               const vk::Format &imageFormat)
    : device(device), imageFormat(imageFormat) {
    createDescriptorSetLayout();
    createGraphicsPipeline();
};

[[nodiscard]]
vk::raii::ShaderModule
VulkanGraphics::createShaderModule(const std::vector<char> &code) const {
    vk::ShaderModuleCreateInfo shaderInfo{};
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    vk::raii::ShaderModule shaderModule{this->device, shaderInfo};

    return shaderModule;
};

std::vector<char> VulkanGraphics::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
};

void VulkanGraphics::createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding(
        0, vk::DescriptorType::eUniformBuffer, 1,
        vk::ShaderStageFlagBits::eVertex, nullptr);

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    this->descriptorSetLayout =
        vk::raii::DescriptorSetLayout{this->device, layoutInfo, nullptr};
};

void VulkanGraphics::createGraphicsPipeline() {
    this->shaderModule = createShaderModule(readFile("shaders/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = this->shaderModule;
    vertShaderStageInfo.pName = "vertMain";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = this->shaderModule;
    fragShaderStageInfo.pName = "fragMain";

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{};
    assemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescription = Vertex::getAttributeDescription();

    vk::PipelineVertexInputStateCreateInfo vertexInfo{};
    vertexInfo.vertexBindingDescriptionCount = 1;
    vertexInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescription.size());
    vertexInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInfo.pVertexAttributeDescriptions = attributeDescription.data();

    vk::PipelineViewportStateCreateInfo viewportStateInfo{};
    viewportStateInfo.pViewports = nullptr; // use dynamic viewport state
    viewportStateInfo.pScissors = nullptr;  // use dunamic scissor state
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.depthClampEnable = vk::False;
    rasterizationInfo.rasterizerDiscardEnable = vk::False;
    rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationInfo.depthBiasEnable = vk::False;
    rasterizationInfo.depthBiasSlopeFactor = 1.0f;
    rasterizationInfo.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multiSamplingInfo{};
    multiSamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multiSamplingInfo.sampleShadingEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.blendEnable = vk::False;
    colorAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorAttachment.srcColorBlendFactor = vk::BlendFactor::eSrc1Alpha;
    colorAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrc1Alpha;
    colorAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.logicOp = vk::LogicOp::eCopy;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorAttachment;

    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &*this->descriptorSetLayout;
    layoutInfo.pushConstantRangeCount = 0;

    this->layout = vk::raii::PipelineLayout{this->device, layoutInfo, nullptr};

    vk::PipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &this->imageFormat;

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pNext = &renderingInfo;
    pipelineInfo.pVertexInputState = &vertexInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportStateInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multiSamplingInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = this->layout;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    this->graphicsPipeline =
        vk::raii::Pipeline{this->device, nullptr, pipelineInfo, nullptr};
};
