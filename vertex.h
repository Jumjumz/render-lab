#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <vector>

class vertex {
  public:
    virtual ~vertex() = default;

    virtual std::vector<vect3> points() = 0;

    virtual std::vector<std::array<int, 2>> lines() = 0;

    // diag connection x: (3, 4), (2, 5) y: (0, 2), (4, 5), z: (0, 5), (3, 6)
};

#endif // !VERTEX_H
