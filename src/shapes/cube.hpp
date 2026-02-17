#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "mesh.hpp"
#include <glm/fwd.hpp>

class Cube : public Mesh {
  public:
    Cube(const float &sides);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

  private:
    static constexpr size_t NUM_VTX = 8;
    static constexpr size_t FACES = 6;
    static constexpr glm::vec3 COLOR = {0.0f, 1.0f, 0.0f};

    std::vector<glm::vec3> vertices;
};

#endif // !CUBE_HPP
