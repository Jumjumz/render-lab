#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <vector>

class vertex {
  public:
    virtual ~vertex() = default;

    virtual std::vector<vect3> points() = 0;

    virtual std::vector<std::array<int, 2>> lines() = 0;
};

#endif // !VERTEX_H
