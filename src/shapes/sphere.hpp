#ifndef SPHERE_HPP
#define SPHERE_HPP

#pragma once

#include "shapes/mesh.hpp"

class Sphere : public Mesh {
  public:
    Sphere(const float &diameter);

    std::vector<Vertex> surfaceInterpolation(const size_t &subdivision) override;

    std::vector<uint16_t> surfaceGrids(const size_t &subdivision) override;

  private:
    float radius = 0.5;

    std::vector<Vertex> points;

    const float phi = (1 + std::sqrt(5)) / 2; // golden ratio
    static constexpr uint MULTIPLIER = 200;
};

#endif // !SPHERE_HPP
