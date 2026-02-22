#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#pragma once

#include "mesh.hpp"

class Pyramid : public Mesh {
  public:
    Pyramid(const float &size);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

  private:
    float size;
    static constexpr size_t NUM_VTX = 5;
    static constexpr size_t FACES = 5;
};

#endif // !PYRAMID_HPP
