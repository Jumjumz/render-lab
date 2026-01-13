#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <cstdint>
#include <vector>

class vertex {
  public:
    virtual ~vertex() = default;

    virtual std::vector<vect3> points() = 0;

    virtual std::vector<std::array<int, 2>> lines() = 0;

    virtual std::vector<vect3>
    surface_interpolation(const uint32_t &subdivision) = 0;
};

#endif // !VERTEX_H
