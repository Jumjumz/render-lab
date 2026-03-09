#ifndef CAT_HPP
#define CAT_HPP

#pragma once

#include "mesh.hpp"

class Cat : public Mesh {
  public:
    Cat(const float &size);

    MeshData surface(const size_t &subdivision) const override;

  private:
    const float &size;
};

#endif // !CAT_HPP
