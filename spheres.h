#ifndef SPHERES_H
#define SPHERES_H

#include "mesh.h"
#include "vect.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
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
        for (size_t i = 0; i < points.size(); i++) {
            std::vector<std::pair<double, size_t>> neighbors;

            for (size_t j = 0; j < points.size(); j++) {
                if (i == j)
                    continue;

                vect3 diff = points[j] - points[i];
                double dist =
                    std::sqrt(diff.x() * diff.x() + diff.y() * diff.y() +
                              diff.z() * diff.z());

                neighbors.push_back({dist, j});
            };

            std::sort(neighbors.begin(), neighbors.end());

            from_to arc;

            for (size_t k = 0; k < subdivision - 2; k++) {
                size_t n = neighbors[k].second;
                arc[0] = i;
                arc[1] = n;

                arcs.push_back(arc);
            }
        };

        return arcs;
    };

  private:
    vect3 val;
    double radius = 0.5;
    const float phi = 1.618f; // golden ratio
    const uint multiplier = 100;

    std::vector<vect3> points;
    std::vector<from_to> arcs;
};

#endif // !SPHERES_H
