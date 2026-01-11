#ifndef VECT_H
#define VECT_H

#include <array>
class vect {
  public:
    std::array<double, 3> e;

    vect() : e{0, 0, 0} {};

    vect(double x, double y) : e{x, y} {};

    vect(double x, double y, double z) : e{x, y, z} {};

    double x() const { return this->e[0]; };
    double y() const { return this->e[1]; };
    double z() const { return this->e[2]; };

    int size() { return this->e.size(); };

    vect operator=(const vect &v) const { return {v.x(), v.y(), v.z()}; };
    double operator[](int i) const { return e[i]; };
    double &operator[](int i) { return e[i]; };

    vect operator+=(vect const &v) {
        e[0] += v.x();
        e[1] += v.y();
        e[2] += v.z();

        return *this;
    }

    vect operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;

        return *this;
    }

    vect operator/=(double t) {
        e[0] /= t;
        e[1] /= t;
        e[2] /= t;

        return *this;
    }
};

// alias for vector 2
using vect2 = vect;
// alias for vector 3
using vect3 = vect;

inline vect operator+(const vect &v, const vect &u) {
    return {v.x() + u.x(), v.y() + u.y(), v.z() + u.z()};
};

inline vect operator-(const vect &v, const vect &u) {
    return {v.x() - u.x(), v.y() - u.y(), v.z() - u.z()};
};

inline vect operator*(const vect &v, const vect &u) {
    return {v.x() * u.x(), v.y() * u.y(), v.z() * u.z()};
}

#endif // !VECT_H
