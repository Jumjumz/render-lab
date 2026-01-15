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
    spheres(double &radius) : radius(radius) { this->radius /= 2; };

    std::vector<vect3> surface_interpolation(const size_t &subdivision) override {
        size_t num_points = subdivision * 50;

        // fibonacci sphere
        for (size_t i = 0; i < num_points; i++) {
            val.e[1] = radius - ((2.0 * i) / num_points - radius);

            float radius_at_y = std::sqrt((radius - (val.e[1] * val.e[1])));
            float theta = 2 * M_PI * (i / phi);

            val.e[0] = std::cos(theta) * radius_at_y;
            val.e[2] = std::sin(theta) * radius_at_y;

            points.push_back(val);
        }

        return points;
    };

    std::vector<from_to> grid(size_t &subdivision) override { return lines; };

  private:
    vect3 val;
    double radius = 0.5;
    const float phi = 1.618f; // golden ratio

    std::vector<vect3> points;
    std::vector<from_to> lines;
};

#endif // !SPHERES_H
