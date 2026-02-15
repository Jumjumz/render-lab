#ifndef RENDER_HPP
#define RENDER_HPP

#pragma once

#include "renderer/vertex.hpp"

#include "shapes/mesh.hpp"
#include <memory>

class Render {
  public:
    Render(const std::shared_ptr<Mesh> &shape);
    // accepts vertices and indices

    std::vector<Vertex> vertices;
    std::vector<from_to> indices;

  private:
};

#endif // !RENDER_HPP
