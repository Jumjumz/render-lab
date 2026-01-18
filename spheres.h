#ifndef SPHERES_H
#define SPHERES_H

#include "mesh.h"
#include "vect.h"
#include <array>
#include <cmath>
#include <vector>

class spheres : public mesh {
  public:
    spheres(double &diameter) { radius = diameter / 2; };

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
        double avg_threshold =
            std::sqrt((4 * M_PI * radius * radius) / points.size());
        double d_threshold = avg_threshold * (subdivision - 3); // threshold
        from_to arc;

        for (size_t i = 0; i < points.size(); i++) {
            for (size_t j = i + 1; j < points.size(); j++) {
                vect3 diff = points[j] - points[i];
                double dist =
                    std::sqrt(diff.x() * diff.x() + diff.y() * diff.y() +
                              diff.z() * diff.z());

                if (dist < d_threshold) {
                    arc[0] = i;
                    arc[1] = j;

                    arcs.push_back(arc);
                }
            }
        };

        return arcs;
    };

  private:
    vect3 val;
    double radius = 0.5;

    const float phi = (1 + std::sqrt(5)) / 2; // golden ratio
    const uint multiplier = 200;

    std::vector<vect3> points;
    std::vector<from_to> arcs;
};

#endif // !SPHERES_H
