#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "mesh.hpp"

class Cube : public Mesh {
  public:
    Cube(const float &sides);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

  private:
    static constexpr size_t NUM_VTX = 8;
    static constexpr size_t FACES = 6;
};

#endif // !CUBE_HPP
