#include "pyramid.hpp"

Pyramid::Pyramid(const float &size) : size(size) {
    Pyramid::halfEdgeData = ControlCage::halfEdgeData(Cages::PYRAMID);
};

MeshData Pyramid::surface(const size_t &subdivision) const {
    auto mesh =
        GENERATE_SURFACE<HalfEdgeData>(Pyramid::halfEdgeData, subdivision);

    // multiply vectors to pyramid size
    for (auto &v : mesh.vertices) {
        v.pos *= this->size;
    }

    return mesh;
};
