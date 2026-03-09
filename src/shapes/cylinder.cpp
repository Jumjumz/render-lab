#include "cylinder.hpp"

Cylinder::Cylinder(const float &radius) : radius(radius) {
    Cylinder::halfEdgeData = ControlCage::halfEdgeData(Cages::CUBE);
};

MeshData Cylinder::surface(const size_t &subdivision) const {
    auto mesh =
        GENERATE_SURFACE<HalfEdgeData>(Cylinder::halfEdgeData, subdivision);

    // transform to cylinder
    for (auto &v : mesh.vertices) {
        const float d = glm::length(glm::vec2(v.pos.x, v.pos.z));

        // calculate x and z
        v.pos.x = (v.pos.x / d) * this->radius;
        v.pos.y *= this->radius;
        v.pos.z = (v.pos.z / d) * this->radius;
    }

    return mesh;
};
