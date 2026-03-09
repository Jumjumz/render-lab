#include "ocean.hpp"
#include "shapes/control_cage.hpp"
#include <chrono>

Ocean::Ocean(const float &size) : size(size) {
    Ocean::halfEdgeData = ControlCage::halfEdgeData(Cages::CUBE);
};

MeshData Ocean::surface(const size_t &subdivision) const {
    auto mesh = GENERATE_SURFACE<HalfEdgeData>(Ocean::halfEdgeData, subdivision);

    // transform to ocean with waves
    for (auto &v : mesh.vertices) {
        v.pos.x *= 3;
        v.pos = oceanWaves(v.pos) * this->size;
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
