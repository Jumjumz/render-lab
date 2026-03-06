#include "ocean.hpp"
#include "control_cage.hpp"
#include <chrono>

Ocean::Ocean(const float &size) : size(size) {};

MeshData Ocean::surface(const size_t &subdivision) const {
    MeshData mesh;

    for (size_t i = 0; i < ControlCage::CubeCage::CAGE_FACES.size(); i++) {
        auto corners = ControlCage::CubeCage::CAGE_FACES[3]; // only use bot face
        auto n = subdivision + 1;

        uint16_t faceOffset = i * (n * n);

        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                auto pt = Geometry::bilinear(corners, j, k, subdivision);

                pt.x *= 3; // transform to rectangle

                pt = oceanWaves(pt) * this->size;

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

glm::vec3 Ocean::oceanWaves(glm::vec3 &points) const {
    static auto start = std::chrono::high_resolution_clock::now();

    auto current = std::chrono::high_resolution_clock::now();

    const float deltaTime =
        std::chrono::duration<float, std::chrono::seconds::period>(current - start)
            .count();

    const float wavelength = this->size * 2;                           // L
    const float speed = 0.3f;                                          // S
    const glm::vec2 direction = glm::normalize(glm::vec2(1.0f, 0.5f)); // D
    const float amplitude = 0.3f;                                      // A
    const float steepness = 0.3f;                                      // Q

    const float k = glm::radians(360.0f) / wavelength;
    const float f = k * (glm::dot(direction, glm::vec2(points.x, points.z)) -
                         speed * deltaTime);

    // calculate
    points.x += steepness * amplitude * direction.x * std::cos(f);
    points.y = amplitude * std::sin(f);
    points.z += steepness * amplitude * direction.y * std::cos(f);

    return points;
};
