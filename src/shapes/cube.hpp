#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "mesh.hpp"

class Cube : public Mesh {
  public:
    Cube(const float &sides);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<from_to> surfaceGrids(const size_t &subdivision) override;

  private:
    static constexpr size_t numVtx = 8;
    static constexpr size_t faces = 6;

    std::vector<glm::vec3> vertices;
};

#endif // !CUBE_HPP
