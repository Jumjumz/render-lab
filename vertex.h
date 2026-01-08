#ifndef VERTEX_H
#define VERTEX_H

#include "vect.h"

class vertex {
  public:
    virtual ~vertex() = default;

    virtual vect3 vertices() const = 0;
};

#endif // !VERTEX_H
