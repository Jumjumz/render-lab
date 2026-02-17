#ifndef MESH_HPP
#define MESH_HPP

#pragma once

#include "renderer/vertex.hpp"

#include <cstddef>
#include <glm/fwd.hpp>
#include <sys/types.h>
#include <vector>

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) = 0;

    virtual std::vector<uint16_t> surfaceGrids(const size_t &subdivision) = 0;

  protected:
    std::vector<glm::vec3> vertices;

    static constexpr glm::vec3 COLOR = {0.0f, 1.0f, 0.0f};
};

#endif // !MESH_HPP
