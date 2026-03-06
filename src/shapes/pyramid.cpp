#include "pyramid.hpp"
#include "control_cage.hpp"

Pyramid::Pyramid(const float &size) : size(size) {};

MeshData Pyramid::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::PyramidCage::CAGE_FACES.size(); i++) {
        const auto corners = ControlCage::PyramidCage::CAGE_FACES[i];
        const auto n = subdivision + 1;

        // get surface grids
        const auto pointsPerFace = n * (subdivision + 2) / 2;
        const uint16_t faceOffset = i * pointsPerFace;

        // interpolate and get grid connection
        for (size_t j = 0; j < n; j++) {
            const auto rowWidth = n - j;
            for (size_t k = 0; k < rowWidth; k++) {
                auto pt = Geometry::barycentric(corners, j, k, subdivision);

                pt *= this->size;

                mesh.vertices.push_back({pt, Pyramid::COLOR});

                // find and connect grid
                const auto current = faceOffset + (j * (2 * n - j + 1)) / 2 + k;

                // connect to right
                if (k < rowWidth - 1) {
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(current + 1);
                }

                // connect to bottom
                if (j < subdivision) {
                    const auto below =
                        faceOffset + ((j + 1) * (2 * n - j)) / 2 + k;
                    mesh.indices.push_back(current);
                    mesh.indices.push_back(below);
                }
            }
        }
    }

    return mesh;
};
