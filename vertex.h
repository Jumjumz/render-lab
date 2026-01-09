#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"
#include <vector>

class vertex {
  public:
    virtual ~vertex() = default;

    virtual std::vector<vect3> vertices() const = 0;
};

#endif // !VERTEX_H
