#include "control_cage.hpp"

HalfEdgeData ControlCage::halfEdgeData() {
    HalfEdgeData mesh;

    for (const auto &v : ControlCage::CubeCage::CAGE_BOUNDARY) {
        mesh.vertex.push_back({.position = v});
    }

    // build half edge of cube
    mesh.halfEdge.resize(ControlCage::CubeCage::HALF_EDGES.size());

    // build faces of cube
    mesh.faces.resize(ControlCage::CubeCage::FACE_START.size());

    // connect pointers
    for (size_t i = 0; i < mesh.halfEdge.size(); i++) {
        auto &data = ControlCage::CubeCage::HALF_EDGES[i];
        mesh.halfEdge[i].vertex = &mesh.vertex[data.vertex];
        mesh.halfEdge[i].twin = &mesh.halfEdge[data.twin];
        mesh.halfEdge[i].next = &mesh.halfEdge[data.next];
        mesh.halfEdge[i].face = &mesh.faces[data.face];
    }

    // connect face entry pointes
    for (size_t i = 0; i < mesh.faces.size(); i++) {
        mesh.faces[i].halfEdge =
            &mesh.halfEdge[ControlCage::CubeCage::FACE_START[i]];
    }

    // connect vertex entry points
    for (size_t i = 0; i < mesh.halfEdge.size(); i++) {
        mesh.halfEdge[i].vertex->outEdge = &mesh.halfEdge[i];
    }

    return mesh;
};
