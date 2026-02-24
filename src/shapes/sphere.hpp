#ifndef SPHERE_HPP
#define SPHERE_HPP

#pragma once

#include "mesh.hpp"

class Sphere : public Mesh {
  public:
    Sphere(const float &radius);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

  private:
    const float radius;

    // create a cube first then transform to sphere
    static constexpr size_t NUM_VTX = 8;
    static constexpr size_t FACES = 6;
};

#endif // !SPHERE_HPP
