#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class vector3 {
  public:
    double e[3];

    vector3() : e{e[0], e[1]} {};

    vector3(double x, double y) : e{x, y} {};

    vector3(double x, double y, double z) : e{x, y, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];
};

#endif // !VECTOR3_H
