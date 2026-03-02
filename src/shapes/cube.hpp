#ifndef CUBE_HPP
#define CUBE_HPP

#pragma once

#include "mesh.hpp"

class Cube : public Mesh {
  public:
    Cube(const float &sides);

    MeshData surface(const size_t &subdivision) override;

  private:
    const float &sides;
};

#endif // !CUBE_HPP
