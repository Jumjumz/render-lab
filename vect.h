#ifndef VECT_H
#define VECT_H

#include <vector>

class vect {
  public:
    std::vector<double> e;

    vect() : e{0, 0, 0} {};

    vect(double x, double y) : e{x, y} {};

    vect(double x, double y, double z) : e{x, y, z} {};

    double x() const { return e[0]; };
    double y() const { return e[1]; };
    double z() const { return e[2]; };

    int size() { return e.size(); };

    vect operator=(const vect &v) const { return {v.x(), v.y(), v.z()}; };
    double operator[](int i) const { return e[i]; };
};

// alias for vector 2
using vect2 = vect;
// alias for vector 3
using vect3 = vect;

#endif // !VECT_H
