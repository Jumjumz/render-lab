#ifndef VERTEX_HPP
#define VERTEX_HPP

#pragma once

#include <array>
#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
    };

    static std::array<vk::VertexInputAttributeDescription, 2>
    getAttributeDescription() {
        return {vk::VertexInputAttributeDescription{
                    0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos)},
                vk::VertexInputAttributeDescription{
                    1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)}};
    };
};

const std::vector<Vertex> vertices = {{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}};

const std::vector<uint16_t> indices = {0, 1, 1, 2, 0, 3, 2, 3};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif // !VERTEX_HPP
