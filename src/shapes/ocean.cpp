#include "ocean.hpp"
#include "control_cage.hpp"
#include <chrono>

Ocean::Ocean(const float &size) : size(size) {};

MeshData Ocean::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CubeCage::CAGE_FACES.size(); i++) {
        auto corners = ControlCage::CubeCage::CAGE_FACES[3];
        auto n = subdivision + 1;

        uint16_t faceOffset = i * (n * n);

        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto pt = Geometry::bilinear(corners, j, k, subdivision);

                pt.y *= 0; // multiply to zero to center the plane
                pt.x *= this->size * 5;
                pt.y += wave(pt.x);     // squish the cube to look like a plane
                pt.z *= this->size * 2; // extend to make it a rectangle

                pt *= this->size;

                mesh.vertices.push_back({pt, Ocean::COLOR});

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

float Ocean::wave(const float &x) const {
    static auto start = std::chrono::high_resolution_clock::now();

    auto current = std::chrono::high_resolution_clock::now();

    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                          current - start)
                          .count();

    return std::sin(x + deltaTime);
};
