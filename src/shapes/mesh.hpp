#ifndef MESH_HPP
#define MESH_HPP

#pragma once

#include "renderer/vertex.hpp"
#include <array>
#include <cstddef>
#include <sys/types.h>
#include <vector>

typedef std::array<uint16_t, 2> from_to;

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) = 0;

    virtual std::vector<from_to> surfaceGrids(const size_t &subdivision) = 0;
};

#endif // !MESH_HPP
