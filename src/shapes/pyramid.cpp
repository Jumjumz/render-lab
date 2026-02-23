#include "pyramid.hpp"

Pyramid::Pyramid(const float &size) : size(size) {
    for (size_t i = 0; i < Pyramid::NUM_VTX; i++) {
        glm::vec3 val;

        // top vertex of pyramid
        if (i == 0)
            val = {0, 1, 0};

        // find the base vertices
        for (size_t j = 0; j < static_cast<size_t>(val.length()); j++) {
            if (j != 1 && i != 0) {
                int axis = (i + 1) & (1 << j);

                val[1] = 0; // y always 0 for base
                if (axis == (1 << j)) {
                    val[j] = -1;
                } else {
                    val[j] = 1;
                }
            }
        }

        this->vertices.push_back(val);
    }
};

std::vector<Vertex> Pyramid::surfaceInterpolation(const size_t &subdivision) {
    std::vector<Vertex> points;
    size_t grid = subdivision + 1;

    for (size_t i = 0; i < Pyramid::FACES; i++) {
        uint cA = i + 1;
        uint cB = i + 2;

        // arranged to correct order of base vertices
        if (i == 1) {
            cB = 4;
        }

        if (i == 3) {
            cA = 3;
            cB = 1;
        }

        for (size_t j = 0; j < grid; j++) {
            glm::vec3 pt;
            for (size_t k = 0; k < grid; k++) {
                float u = (float)j / subdivision;
                float v = (float)k / subdivision;

                // calculate for the base
                if (i == 4) {
                    glm::vec3 tE =
                        ((1 - u) * this->vertices[1]) + (u * this->vertices[2]);
                    glm::vec3 bE =
                        ((1 - u) * this->vertices[3]) + (u * this->vertices[4]);

                    pt = ((1 - v) * tE) + (v * bE);
                } else {
                    if (u + v < 1.0f) {
                        pt = ((1 - u - v) * this->vertices[0]) +
                             (u * this->vertices[cA]) + (v * this->vertices[cB]);
                    } else {
                        continue;
                    }
                }

                pt *= this->size;

                points.push_back({pt, Pyramid::COLOR});
            }
        }
    }

    return points;
};

std::vector<uint16_t> Pyramid::surfaceGrids(const size_t &subdivision) {
    uint pointsPerFace = (subdivision + 1) * (subdivision + 1);
    uint pointsPerRow = subdivision + 1;

    // TODO: needs to be updated
    std::vector<uint16_t> lines;
    for (size_t i = 0; i < Pyramid::FACES; i++) {
        uint faceStart = i * pointsPerFace; // offset to this face vertices

        for (size_t j = 0; j < pointsPerFace; j++) {
            uint n = faceStart + j;
            uint row = j / pointsPerRow;
            uint col = j % pointsPerRow;

            if (col < subdivision) {
                lines.push_back(n);
                lines.push_back(n + 1); // point to the right
            }

            if (row < subdivision) {
                lines.push_back(n);
                lines.push_back(n + pointsPerRow); // point below
            }
        }
    }

    return lines;
};
