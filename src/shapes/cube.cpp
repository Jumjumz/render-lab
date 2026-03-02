#include "cube.hpp"
#include "control_cage.hpp"

Cube::Cube(const float &sides) : sides(sides) {
    for (auto v : ControlCage::CAGE_BOUNDARY) {
        this->vertices.push_back(v);
    }

    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        size_t h = ControlCage::FACE_START[i];
        size_t start = h;

        std::vector<uint16_t> faceVerts;
        do {
            faceVerts.push_back(ControlCage::HALF_EDGES[h].vertex);

            h = ControlCage::HALF_EDGES[h].next;
        } while (h != start);
    }
};

std::vector<Vertex> Cube::surfaceInterpolation(const size_t &subdivision) {
    std::vector<Vertex> surfacePoints;

    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        std::array<uint16_t, 4> corners = ControlCage::CAGE_FACES[i];

        // interpolate
        for (size_t j = 0; j <= subdivision; j++) {
            for (size_t k = 0; k <= subdivision; k++) {
                float u = static_cast<float>(j) / subdivision;
                float v = static_cast<float>(k) / subdivision;

                auto p0 = this->vertices[corners[0]];
                auto p1 = this->vertices[corners[1]];
                auto p2 = this->vertices[corners[2]];
                auto p3 = this->vertices[corners[3]];

                // Bilinear interpolation
                glm::vec3 pt = (1 - u) * (1 - v) * p0 + u * (1 - v) * p1 +
                               u * v * p2 + (1 - u) * v * p3;

                pt *= this->sides;

                surfacePoints.push_back({pt, Cube::COLOR});
            }
        }
    }

    return surfacePoints;
};

std::vector<uint16_t> Cube::surfaceGrids(const size_t &subdivision) {
    auto n = subdivision + 1;
    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        // build indices
        uint16_t faceOffset = i * (n * n);
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto current = faceOffset + (j * n) + k;

                if (k < subdivision) {
                    auto right = current + 1;
                    this->indices.push_back(current);
                    this->indices.push_back(right);
                }

                if (j < subdivision) {
                    auto bottom = current + n;
                    this->indices.push_back(current);
                    this->indices.push_back(bottom);
                }
            }
        }
    }

    return this->indices;
};
