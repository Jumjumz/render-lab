#ifndef VECTOR3_H
#define VECTOR3_H

class vec {
  public:
    double e[3];

    vec() : e{e[0], e[1]} {};

    vec(double x, double y) : e{x, y} {};

    vec(double x, double y, double z) : e{x, y, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];

    vec operator=(const vec &v) const { return {v.x, v.y, v.z}; };
};

#endif // !VECTOR3_H
