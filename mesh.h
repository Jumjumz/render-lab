#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <array>
#include <cstdint>
#include <vector>

typedef std::array<int, 2> from_to;

class mesh {
  public:
    virtual ~mesh() = default;

    virtual std::vector<vect3> points() = 0;

    virtual std::vector<from_to> lines() = 0;

    virtual std::vector<vect3>
    surface_interpolation(const uint32_t &subdivision) = 0;

    virtual std::vector<from_to> surface_lines() = 0;
};

#endif // !VERTEX_H
