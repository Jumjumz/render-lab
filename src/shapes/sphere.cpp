#include "sphere.hpp"

Sphere::Sphere(const float &radius) : radius(radius) {
    // find the sides of the cube
    for (size_t i = 0; i < Sphere::NUM_VTX; i++) {
        glm::vec3 val;
        for (size_t j = 0; j < static_cast<size_t>(val.length()); j++) {
            int axis = i & (1 << j);

            if (axis == (1 << j)) {
                val[j] = -1;
            } else {
                val[j] = 1;
            }
        }

        this->vertices.push_back(val);
    }
};

std::vector<Vertex> Sphere::surfaceInterpolation(const size_t &subdivision) {
    std::vector<Vertex> points;

    // interpolate the cube
    for (size_t i = 0; i < Sphere::FACES; i++) {
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
                glm::vec3 pts = (1 - u) * (1 - v) * corners[0] +
                                u * (1 - v) * corners[1] + u * v * corners[2] +
                                (1 - u) * v * corners[3];

                points.push_back({pts, Sphere::COLOR});
            }
        }
    }

    // turn cube into sphere
    std::vector<Vertex> sp = points;

    for (size_t i = 0; i < sp.size(); i++) {
        float x = points[i].pos[0];
        float y = points[i].pos[1];
        float z = points[i].pos[2];

        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;

        float xP = x * std::sqrt(1 - (y2 / 2) - (z2 / 2) + (y2 * z2 / 3));
        float yP = y * std::sqrt(1 - (x2 / 2) - (z2 / 2) + (x2 * z2 / 3));
        float zP = z * std::sqrt(1 - (x2 / 2) - (y2 / 2) + (x2 * y2 / 3));

        // new values
        sp[i].pos[0] = xP;
        sp[i].pos[1] = yP;
        sp[i].pos[2] = zP;

        // and normalized with radius
        sp[i].pos *= this->radius;
    }

    return sp;
};

std::vector<uint16_t> Sphere::surfaceGrids(const size_t &subdivision) {
    uint pointsPerFace = (subdivision + 1) * (subdivision + 1);
    uint pointsPerRow = subdivision + 1;

    // similar to searching grid in cube
    std::vector<uint16_t> arcs;
    for (size_t i = 0; i < Sphere::FACES; i++) {
        uint faceStart = i * pointsPerFace; // offset to this face vertices

        for (size_t j = 0; j < pointsPerFace; j++) {
            uint n = faceStart + j;
            uint row = j / pointsPerRow;
            uint col = j % pointsPerRow;

            if (col < subdivision) {
                arcs.push_back(n);
                arcs.push_back(n + 1); // point to the right
            }

            if (row < subdivision) {
                arcs.push_back(n);
                arcs.push_back(n + pointsPerRow); // point below
            }
        }
    }

    return arcs;
};
