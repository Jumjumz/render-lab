#ifndef MESH_HPP
#define MESH_HPP

#pragma once

#include "renderer/vertex.hpp"

#include <cstddef>
#include <glm/fwd.hpp>
#include <sys/types.h>
#include <vector>

struct MeshData;

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual MeshData surface(const size_t &subdivision) const = 0;

    static constexpr glm::vec3 COLOR = {0.0f, 1.0f, 0.0f};
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

#endif // !MESH_HPP
