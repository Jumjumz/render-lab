#include "cube.hpp"

Cube::Cube(const float &sides) : sides(sides) {
    Cube::halfEdgeData = ControlCage::halfEdgeData(Cages::CUBE);
};

MeshData Cube::surface(const size_t &subdivision) const {
    auto mesh = GENERATE_SURFACE<HalfEdgeData>(Cube::halfEdgeData, subdivision);

    // multiply vectors to cube sides
    for (auto &v : mesh.vertices) {
        v.pos *= this->sides;
    }

    return mesh;
}
