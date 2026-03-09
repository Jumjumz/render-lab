#include "cat.hpp"

Cat::Cat(const float &size) : size(size) {
    Cat::halfEdgeData = ControlCage::halfEdgeData(Cages::CUBE);
};

MeshData Cat::surface(const size_t &subdivision) const {
    auto mesh = GENERATE_SURFACE(Cat::halfEdgeData, subdivision);

    for (auto &v : mesh.vertices) {
        v.pos = glm::normalize(v.pos) * this->size;
        v.pos.x *= 1.5;
        v.pos.z *= 1.5;
    }

    return mesh;
};

// TODO:: complete the cat render
