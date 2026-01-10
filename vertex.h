#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <vector>

class vertex {
  public:
    virtual ~vertex() = default;

    virtual std::vector<vect3> points() const = 0;

    virtual std::vector<std::vector<int>> lines() const = 0;
};

#endif // !VERTEX_H
