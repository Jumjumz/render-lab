#include "cube.hpp"
#include "control_cage.hpp"

Cube::Cube(const float &sides) : sides(sides) {};

MeshData Cube::surface(const size_t &subdivision) {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        std::array<uint16_t, 4> corners = ControlCage::CAGE_FACES[i];
        auto n = subdivision + 1;

        // interpolate and get points
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                float u = static_cast<float>(j) / subdivision;
                float v = static_cast<float>(k) / subdivision;

                auto p0 = ControlCage::CAGE_BOUNDARY[corners[0]];
                auto p1 = ControlCage::CAGE_BOUNDARY[corners[1]];
                auto p2 = ControlCage::CAGE_BOUNDARY[corners[2]];
                auto p3 = ControlCage::CAGE_BOUNDARY[corners[3]];

                // Bilinear interpolation
                glm::vec3 pt = (1 - u) * (1 - v) * p0 + u * (1 - v) * p1 +
                               u * v * p2 + (1 - u) * v * p3;

                pt *= this->sides;

                mesh.vertices.push_back({pt, Cube::COLOR});
            }
        }

        // get surface grids
        uint16_t faceOffset = i * (n * n);

        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                size_t current = faceOffset + (j * n) + k;

                if (k < subdivision) {
                    auto right = current + 1;
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(right);
                }

                if (j < subdivision) {
                    auto bottom = current + n;
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(bottom);
                }
            }
        }
    }

    return mesh;
}
