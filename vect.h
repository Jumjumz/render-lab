#ifndef VECT_H
#define VECT_H

class vect {
  public:
    double e[3];

    vect() : e{e[0], e[1]} {};

    vect(double x, double y) : e{x, y} {};

    vect(double x, double y, double z) : e{x, y, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];

    vect operator=(const vect &v) const { return {v.x, v.y, v.z}; };
};

// alias for vector 2
using vect2 = vect;
// alias for vector 3
using vect3 = vect;

#endif // !VECT_H
