#include "cylinder.hpp"
#include "control_cage.hpp"

Cylinder::Cylinder(const float &radius) : radius(radius) {};

MeshData Cylinder::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CubeCage::CAGE_FACES.size(); i++) {
        std::array<uint16_t, 4> corners = ControlCage::CubeCage::CAGE_FACES[i];
        auto n = subdivision + 1;
        // get surface grids
        uint16_t faceOffset = i * (n * n);

        // interpolate and get grid connection
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto pt = Geometry::bilinear(corners, j, k, subdivision);

                // transform to cylinder
                float d = glm::length(glm::vec2(pt.x, pt.z));

                // calculate for x and z
                pt.x = (pt.x / d) * this->radius;
                pt.y *= this->radius;
                pt.z = (pt.z / d) * this->radius;

                mesh.vertices.push_back({pt, Cylinder::COLOR});

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
