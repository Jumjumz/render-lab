#ifndef MESH_H
#define MESH_H

#include "vect.h"
#include <array>
#include <cstddef>
#include <sys/types.h>
#include <vector>

typedef std::array<uint, 2> from_to;

class mesh {
  public:
    virtual ~mesh() = default;

    virtual std::vector<vect3> surface_interpolation(const size_t &subdivision) = 0;

    virtual std::vector<from_to> grid(size_t &subdivision) = 0;
};

#endif // !MESH_H
