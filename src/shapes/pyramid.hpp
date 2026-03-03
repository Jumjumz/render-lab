#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#pragma once

#include "mesh.hpp"

class Pyramid : public Mesh {
  public:
    Pyramid(const float &size);

    MeshData surface(const size_t &subdivision) const override;

  private:
    const float &size;
};

#endif // !PYRAMID_HPP
