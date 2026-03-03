#include "sphere.hpp"
#include "control_cage.hpp"

Sphere::Sphere(const float &radius) : radius(radius) {};

MeshData Sphere::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CubeCage::CAGE_FACES.size(); i++) {
        std::array<uint16_t, 4> corners = ControlCage::CubeCage::CAGE_FACES[i];
        auto n = subdivision + 1;

        // interpolate and get points
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto pt = Geometry::bilinear(corners, j, k, subdivision);

                // transform to a sphere
                pt = glm::normalize(pt) * this->radius;

                mesh.vertices.push_back({pt, Sphere::COLOR});
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
