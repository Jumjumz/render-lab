#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#pragma once

#include "mesh.hpp"

class Cylinder : public Mesh {
  public:
    Cylinder(const float &radius);

    MeshData surface(const size_t &subdivision) const override;

  private:
    const float &radius;
};

#endif // !CYLINDER_HPP
