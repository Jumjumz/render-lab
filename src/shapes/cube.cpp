#include "cube.hpp"
#include <cstdint>

Cube::Cube(const float &sides) {
    for (size_t i = 0; i < Cube::NUM_VTX; i++) {
        glm::vec3 val;
        for (size_t j = 0; j < static_cast<size_t>(val.length()); j++) {
            int axis = i & (1 << j);

            if (axis == (1 << j)) {
                val[j] = -sides / 2;
            } else {
                val[j] = sides / 2;
            }
        }

        this->vertices.push_back(val);
    }
};

std::vector<Vertex> Cube::surfaceInterpolation(const size_t &subdivision) {
    std::vector<Vertex> surfacePoints;

    for (size_t i = 0; i < Cube::FACES; i++) {
        uint axis = i / 2;  // x = 0; y; 1; z; 2
        uint value = i % 2; // min = 0; max = 1 -> identify the i of
                            // which axis we are in the loop

        std::vector<glm::vec3> corners;

        // find each corners
        for (size_t i = 0; i < this->vertices.size(); i++) {
            if (((i >> axis) & 1) == value) {
                corners.push_back(this->vertices[i]);
            }
        }

        // sort by other axes
        uint axis1 = (axis + 1) % 3;
        uint axis2 = (axis + 2) % 3;

        std::sort(corners.begin(), corners.end(),
                  [axis1, axis2](const glm::vec3 a, const glm::vec3 b) -> bool {
                      double a1 = a[axis1], a2 = a[axis1];
                      double b1 = b[axis2], b2 = b[axis2];

                      if (a1 != b1)
                          return a1 < b1;
                      return a2 < b2;
                  });

        // interpolate
        for (size_t i = 0; i <= subdivision; i++) {
            for (size_t j = 0; j <= subdivision; j++) {
                float u = static_cast<float>(i) / subdivision;
                float v = static_cast<float>(j) / subdivision;

                // Bilinear interpolation
                glm::vec3 pt = (1 - u) * (1 - v) * corners[0] +
                               u * (1 - v) * corners[1] + u * v * corners[2] +
                               (1 - u) * v * corners[3];

                surfacePoints.push_back({pt, Cube::COLOR});
            }
        }
    }

    return surfacePoints;
};

std::vector<uint16_t> Cube::surfaceGrids(const size_t &subdivision) {
    uint pointsPerFace = (subdivision + 1) * (subdivision + 1);
    uint pointsPerRow = subdivision + 1;

    std::vector<uint16_t> lines;
    for (size_t i = 0; i < Cube::FACES; i++) {
        uint faceStart = i * pointsPerFace;

        for (size_t j = 0; j < pointsPerFace; j++) {
            uint n = faceStart + j;
            uint row = j / pointsPerRow;
            uint col = j % pointsPerRow;

            if (col < subdivision) {
                lines.push_back(n);
                lines.push_back(n + 1);
            }

            if (row < subdivision) {
                lines.push_back(n);
                lines.push_back(n + pointsPerRow);
            }
        }
    }

    return lines;
};
