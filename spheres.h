#ifndef SPHERES_H
#define SPHERES_H

#include "mesh.h"
#include "vect.h"
#include <cmath>
#include <cstddef>
#include <sys/types.h>
#include <vector>

class spheres : public mesh {
  public:
    spheres(double &radius) : radius(radius) {
        // this is fibonacci sphere
        float phi = 1.618; // golden retio
        radius /= 2;

        for (size_t i = 0; i < num; i++) {
            val.e[1] = radius - ((2.0 * i) / num - radius);

            float radius_at_y = std::sqrt((radius - (val.e[1] * val.e[1])));
            float theta = 2 * M_PI * (i / phi);

            val.e[0] = std::cos(theta) * radius_at_y;
            val.e[2] = std::sin(theta) * radius_at_y;

            vertices.push_back(val);
        }
    };

    std::vector<vect3> surface_interpolation(const uint &subdivision) override {
        return vertices;
    };

    std::vector<from_to> grid(uint &subdivision) override { return lines; };

  private:
    vect3 val;
    double radius = 0.5;
    uint num = 400;

    std::vector<vect3> vertices;

    std::vector<vect3> points;
    std::vector<from_to> lines;
};

#endif // !SPHERES_H
