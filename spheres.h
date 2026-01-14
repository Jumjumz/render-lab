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
    spheres(double radius) : radius(radius) {
        uint n = num * 2;
        float phi;
        float theta;
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                phi = M_PI * (double(i) / num);
                theta = 2 * M_PI * (double(j) / num);

                val.e[0] = radius * std::sin(phi) * std::cos(theta);
                val.e[1] = radius * std::cos(phi);
                val.e[2] = radius * std::sin(phi) * std::sin(theta);

                vertices.push_back(val);
            }
        }
    };

    std::vector<vect3> surface_interpolation(const uint &subdivision) override {
        return vertices;
    };

    std::vector<from_to> grid(uint &subdivision) override { return lines; };

  private:
    vect3 val;
    double radius = 0.5;
    uint num = 4;

    std::vector<vect3> vertices;

    std::vector<vect3> points;
    std::vector<from_to> lines;
};

#endif // !SPHERES_H
