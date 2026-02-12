#include "render_lab.hpp"
#include "vertex.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

RenderLab::RenderLab() {};

void RenderLab::run() {
    loop();
    cleanUp();
};

// camera/model/projection setup
void RenderLab::setup() {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();

    // delta time
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                          currentTime - startTime)
                          .count();

    // rotation and camera perspective
    UniformBufferObject ubo{
        .model = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(45.0f),
                             glm::vec3(0.0f, 0.0f, 1.0f)),
        .view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f)),
        .proj = glm::perspective(glm::radians(45.0f), this->window.aspect_ratio,
                                 0.1f, 10.0f)};

    // y flip
    ubo.proj[1][1] *= -1;

    memcpy(this->resources.uniformBuffersMapped[this->currentFrame], &ubo,
           sizeof(ubo));
};

void RenderLab::drawFrame() {
    auto fenceResult = this->ctx.device.waitForFences(
        *this->commands.inFlightFences[this->currentFrame], vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess)
        throw std::runtime_error("Failed to wait for fence!");

    auto [result, imageIndex] = this->swapchain.swapchain.acquireNextImage(
        UINT64_MAX, *this->commands.availableSemaphores[this->currentFrame],
        nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        this->swapchain.recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess &&
               result != vk::Result::eSuboptimalKHR) {
        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    this->ctx.device.resetFences(
        *this->commands.inFlightFences[this->currentFrame]);

    this->commands.commandBuffers[this->currentFrame].reset();

    recordCommandBuffer(imageIndex);

    setup();

    vk::PipelineStageFlags destinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1,
    submitInfo.pWaitSemaphores =
        &*this->commands.availableSemaphores[this->currentFrame];
    submitInfo.pWaitDstStageMask = &destinationStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers =
        &*this->commands.commandBuffers[this->currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores =
        &*this->commands.finishedSemaphores[this->currentFrame];

    this->ctx.graphicsQueue.submit(
        submitInfo, *this->commands.inFlightFences[this->currentFrame]);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores =
        &*this->commands.finishedSemaphores[this->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &*this->swapchain.swapchain;
    presentInfo.pImageIndices = &imageIndex;

    result = this->ctx.presentQueue.presentKHR(presentInfo);

    if ((result == vk::Result::eSuboptimalKHR) ||
        (result == vk::Result::eErrorOutOfDateKHR) || framebufferResized) {
        this->framebufferResized = false;
        this->swapchain.recreateSwapChain();
    } else {
        assert(result == vk::Result::eSuccess);
    }

    this->currentFrame =
        (this->currentFrame + 1) % RenderLab::MAX_FRAMES_IN_FLIGHT;
};

void RenderLab::recordCommandBuffer(uint32_t imageIndex) {
    auto &cmd = this->commands.commandBuffers[this->currentFrame];

    cmd.begin({});

    transitionImageLayout(imageIndex, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eColorAttachmentOptimal, {},
                          vk::AccessFlagBits2::eColorAttachmentWrite,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    // set up color attachment
    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderingAttachmentInfo attachmentInfo{};
    attachmentInfo.imageView = this->swapchain.resources.imageViews[imageIndex];
    attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentInfo.clearValue = clearColor;

    vk::Offset2D offset = {0, 0};

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea.offset = offset;
    renderingInfo.renderArea.extent = this->swapchain.resources.extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &attachmentInfo;

    cmd.beginRendering(renderingInfo);

    // render commands
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
                     this->graphics.graphicsPipeline);

    cmd.bindVertexBuffers(0, *this->resources.vertexBuffer, {0});

    cmd.bindIndexBuffer(*this->resources.indexBuffer, 0, vk::IndexType::eUint16);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, this->graphics.layout, 0,
        *this->commands.descriptorSets[this->currentFrame], nullptr);

    cmd.setViewport(
        0,
        vk::Viewport{0.0f, 0.0f,
                     static_cast<float>(this->swapchain.resources.extent.width),
                     static_cast<float>(this->swapchain.resources.extent.height),
                     0.0f, 1.0f});

    cmd.setScissor(
        0, vk::Rect2D{vk::Offset2D{0, 0}, this->swapchain.resources.extent});

    cmd.drawIndexed(indices.size(), 1, 0, 0, 0);

    cmd.endRendering();

    transitionImageLayout(imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ImageLayout::ePresentSrcKHR,
                          vk::AccessFlagBits2::eColorAttachmentWrite, {},
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eBottomOfPipe);

    cmd.end();
};

void RenderLab::transitionImageLayout(uint32_t imageIndex,
                                      vk::ImageLayout oldLayout,
                                      vk::ImageLayout newLayout,
                                      vk::AccessFlags2 srcAccessMask,
                                      vk::AccessFlags2 dstAccessMask,
                                      vk::PipelineStageFlags2 srcStageMask,
                                      vk::PipelineStageFlags2 dstStageMask) {
    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcStageMask = srcStageMask;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstStageMask = dstStageMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.image = this->swapchain.resources.images[imageIndex];
    barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    this->commands.commandBuffers[this->currentFrame].pipelineBarrier2(
        dependencyInfo);
};

void RenderLab::loop() {
    while (this->window.running) {
        while (SDL_PollEvent(&this->window.event)) {
            if (this->window.event.type == SDL_QUIT)
                this->window.running = false;

            if (this->window.event.type == SDL_WINDOWEVENT_RESIZED)
                this->framebufferResized = true;
        }

        drawFrame();
    }

    this->ctx.device.waitIdle();
};

void RenderLab::cleanSwapchain() {
    this->swapchain.resources.imageViews.clear();
    this->swapchain.swapchain = nullptr;
};

void RenderLab::cleanUp() {
    cleanSwapchain();

    this->window.destroy();
};
