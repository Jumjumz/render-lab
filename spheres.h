#ifndef SPHERES_H
#define SPHERES_H

#include "mesh.h"
#include "vect.h"
#include <cmath>
#include <vector>

class spheres : public mesh {
  public:
    spheres(double &radius) : radius(radius) { this->radius /= 2; };

    std::vector<vect3> surface_interpolation(const size_t &subdivision) override {
        size_t num_points = subdivision * multiplier;

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

    std::vector<from_to> grid(size_t &subdivision) override {
        from_to arcs;

        for (size_t i = 0; i < points.size(); i++) {
            size_t offsets[4] = {1, 8, 21, 34};
            for (uint offset : offsets) {
                size_t n = i + offset;

                if (n < points.size()) {
                    arcs[0] = i;
                    arcs[1] = n;

                    lines.push_back(arcs);
                }
            }
        }

        return lines;
    };

  private:
    vect3 val;
    double radius = 0.5;
    const float phi = 1.618f; // golden ratio
    const uint multiplier = 50;

    std::vector<vect3> points;
    std::vector<from_to> lines;
};

#endif // !SPHERES_H
