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
    // predefined cube
    static constexpr std::array<glm::vec3, 8> CAGE_BOUNDARY = {{{-1, -1, -1},
                                                                {1, -1, -1},
                                                                {1, 1, -1},
                                                                {-1, 1, -1},
                                                                {-1, -1, 1},
                                                                {1, -1, 1},
                                                                {1, 1, 1},
                                                                {-1, 1, 1}}};
    static constexpr std::array<std::array<uint16_t, 4>, 6> CAGE_FACES = {{
        {0, 1, 3, 2},
        {4, 5, 7, 6},
        {2, 3, 6, 7},
        {0, 1, 4, 5},
        {1, 2, 5, 6},
        {0, 2, 4, 7},
    }};
};

#endif // !MESH_HPP
