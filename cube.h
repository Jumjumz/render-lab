#ifndef CUBE_H
#define CUBE_H

#include "mesh.h"
#include "vect.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <sys/types.h>
#include <vector>

class cube : public mesh {
  public:
    cube(double sides) : sides(sides) {
        // use bit & operator to identify the 1.. if a bit is 1 it is value is negative
        for (int i = 0; i < num_vtx; i++) {
            for (int j = 0; j < val.size(); j++) {
                int b = i & (1 << j);

                if (b == (1 << j)) {
                    val.e[j] = -sides / 2;
                } else {
                    val.e[j] = sides / 2;
                }
            }

            vertices.push_back(val);
        }
    };

    std::vector<vect3> surface_interpolation(const uint32_t &subdivision) override {
        for (int face = 0; face < faces; face++) {
            int axis = face / 2;  // x = 0; y; 1; z; 2
            int value = face % 2; // min = 0; max = 1 -> identify the face of
                                  // which axis we are in the loop

            std::vector<vect3> corners;

            // find each corners
            for (int i = 0; i < vertices.size(); i++) {
                if (((i >> axis) & 1) == value) {
                    corners.push_back(vertices[i]);
                }
            }

            // sort by other axes
            int axis1 = (axis + 1) % 3;
            int axis2 = (axis + 2) % 3;

            std::sort(corners.begin(), corners.end(),
                      [axis1, axis2](const vect3 a, const vect3 b) -> bool {
                          double a1 = a.e[axis1], a2 = a.e[axis1];
                          double b1 = b.e[axis2], b2 = b.e[axis2];

                          if (a1 != b1)
                              return a1 < b1;
                          return a2 < b2;
                      });

            // interpolate
            for (int i = 0; i <= subdivision; i++) {
                for (int j = 0; j <= subdivision; j++) {
                    double u = double(i) / subdivision;
                    double v = double(j) / subdivision;

                    // Bilinear interpolation
                    vect3 pt = (1 - u) * (1 - v) * corners[0] +
                               u * (1 - v) * corners[1] + u * v * corners[2] +
                               (1 - u) * v * corners[3];

                    surface_points.push_back(pt);
                }
            }
        }

        return surface_points;
    }

    std::vector<from_to> grid(uint32_t &subdivision) override {
        int points_per_face = (subdivision + 1) * (subdivision + 1);
        int points_per_row = subdivision + 1;
        from_to pt;

        for (int face = 0; face < faces; face++) {
            int face_start = face * points_per_face;

            for (int i = 0; i < points_per_face; i++) {
                int n = face_start + i;
                int row = i / points_per_row;
                int col = i % points_per_row;

                if (col < subdivision) {
                    pt[0] = n;
                    pt[1] = n + 1;

                    lines.push_back(pt);
                }

                if (row < subdivision) {
                    pt[0] = n;
                    pt[1] = n + points_per_row;

                    lines.push_back(pt);
                }
            }
        }

        return lines;
    }

  private:
    vect3 val;
    double sides = 0.5;

    const uint32_t num_vtx = 8;
    const uint32_t faces = 6;

    std::vector<vect3> vertices;
    std::vector<vect3> surface_points;
    std::vector<from_to> lines;
};

#endif // !CUBE_H
