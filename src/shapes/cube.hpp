#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "mesh.hpp"

class Cube : public Mesh {
  public:
    Cube(const float &sides);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

    MeshData surface(const size_t &subdivision) override;

  private:
    const float sides;
};

#endif // !CUBE_HPP
