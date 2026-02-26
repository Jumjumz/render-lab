#ifndef RENDER_HPP
#define RENDER_HPP

#pragma once

#include "vertex.hpp"

enum class Shapes { CUBE, SPHERE, PYRAMID };

class Render {
  public:
    Render();

    struct RenderData {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    } renderData;

    static constexpr glm::vec3 CAMERA = {1.5f, 1.5f, 1.5f};

    RenderData renderShape(const Shapes &render) const;

  private:
    static constexpr size_t SUBDIVISION = 8;
    static constexpr float SIZE = 0.5;
};

#endif // !RENDER_HPP
