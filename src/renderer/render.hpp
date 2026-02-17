#ifndef RENDER_HPP
#define RENDER_HPP

#pragma once

#include "renderer/vertex.hpp"
#include "shapes/mesh.hpp"

#include <memory>

class Render {
  public:
    Render(const std::shared_ptr<Mesh> &shape);

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

  private:
    static constexpr size_t SUBDIVISION = 8;
};

#endif // !RENDER_HPP
