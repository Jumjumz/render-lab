#include "ocean.hpp"
#include "control_cage.hpp"

Ocean::Ocean(const float &size) : size(size) {};

MeshData Ocean::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CubeCage::CAGE_FACES.size(); i++) {
        auto corners = ControlCage::CubeCage::CAGE_FACES[i];
        auto n = subdivision + 1;

        uint16_t faceOffset = i * (n * n);

        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto pt = Geometry::bilinear(corners, j, k, subdivision);

                pt.x *= this->size;
                pt.y *= this->size / 50;
                pt.z *= this->size * 2;

                mesh.vertices.push_back({pt, Ocean::COLOR});

                // find and connect grid
                auto current = faceOffset + (j * n) + k;

                // connect to right
                if (k < subdivision) {
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(current + 1);
                }

                // connect to bottom
                if (j < subdivision) {
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(current + n);
                }
            }
        }
    }

    return mesh;
};
