#include "sphere.hpp"

Sphere::Sphere(const float &radius) : radius(radius) {
    Sphere::halfEdgeData = ControlCage::halfEdgeData(Cages::CUBE);
};

MeshData Sphere::surface(const size_t &subdivision) const {
    auto mesh = GENERATE_SURFACE<HalfEdgeData>(Sphere::halfEdgeData, subdivision);

    // transform cube cage into sphere
    for (auto &v : mesh.vertices) {
        v.pos = glm::normalize(v.pos) * this->radius;
    }

    return mesh;
}
