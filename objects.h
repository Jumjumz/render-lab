#ifndef OBJECTS_H
#define OBJECTS_H

#include "vertex.h"

class cube : vertex {
  public:
    double sides = 0.25;

    cube(double &sides) : sides(sides) {};

    vect3 vertices() const override {

    };

  private:
};

#endif // !OBJECTS_H
