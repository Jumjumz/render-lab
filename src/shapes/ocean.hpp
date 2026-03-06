#ifndef OCEAN_HPP
#define OCEAN_HPP

#pragma once

#include "mesh.hpp"

class Ocean : public Mesh {
  public:
    Ocean(const float &size);

    MeshData surface(const size_t &subdivision) const override;

  private:
    const float &size;

    glm::vec3 oceanWaves(glm::vec3 &points) const;
};

#endif // !OCEAN_HPP
