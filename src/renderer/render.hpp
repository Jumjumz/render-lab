#ifndef RENDER_HPP
#define RENDER_HPP

#pragma once

#include "shapes/mesh.hpp"
#include "vertex.hpp"

#include <memory>

class Render {
  public:
    Render(const std::shared_ptr<Mesh> &shape);

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    static constexpr glm::vec3 CAMERA = {1.5f, 1.5f, 1.5f};

  private:
    static constexpr size_t SUBDIVISION = 8;
};

#endif // !RENDER_HPP
