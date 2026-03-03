#ifndef SPHERE_HPP
#define SPHERE_HPP

#pragma once

#include "mesh.hpp"

class Sphere : public Mesh {
  public:
    Sphere(const float &radius);

    MeshData surface(const size_t &subdivision) const override;

  private:
    const float &radius;
};

#endif // !SPHERE_HPP
