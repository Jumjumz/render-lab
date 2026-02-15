#ifndef VERTEX_HPP
#define VERTEX_HPP

#pragma once

#include <array>
#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

struct Vertex {
    glm::vec3 pos;
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

// for viewing/displaying.
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif // !VERTEX_HPP
