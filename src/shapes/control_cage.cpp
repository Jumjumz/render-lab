#include "control_cage.hpp"

HalfEdgeData ControlCage::halfEdgeData(const Cages &cage) {
    HalfEdgeData mesh;

    switch (cage) {
    case Cages::CUBE: {
        for (const auto &vertex : ControlCage::CubeCage::CAGE_BOUNDARY) {
            mesh.vertex.push_back({.position = vertex});
        }

        // build half edge of cube
        mesh.halfEdge.resize(ControlCage::CubeCage::HALF_EDGES.size());

        // build faces of cube
        mesh.faces.resize(ControlCage::CubeCage::FACE_START.size());

        // connect pointers
        for (size_t i = 0; i < mesh.halfEdge.size(); i++) {
            const auto &data = ControlCage::CubeCage::HALF_EDGES[i];
            mesh.halfEdge[i].vertex = &mesh.vertex[data.vertex];
            mesh.halfEdge[i].twin = &mesh.halfEdge[data.twin];
            mesh.halfEdge[i].next = &mesh.halfEdge[data.next];
            mesh.halfEdge[i].face = &mesh.faces[data.face];

            // connect vertex entry points
            mesh.halfEdge[i].vertex->outEdge = &mesh.halfEdge[i];
        }

        // connect face entry points
        for (size_t i = 0; i < mesh.faces.size(); i++) {
            mesh.faces[i].halfEdge =
                &mesh.halfEdge[ControlCage::CubeCage::FACE_START[i]];
        }

        break;
    }
    case Cages::PYRAMID: {
        for (const auto &vertex : ControlCage::PyramidCage::CAGE_BOUNDARY) {
            mesh.vertex.push_back({.position = vertex});
        }

        // build half edge of pyramid
        mesh.halfEdge.resize(ControlCage::PyramidCage::HALF_EDGES.size());

        // build faces of pyramid
        mesh.faces.resize(ControlCage::PyramidCage::FACE_START.size());

        // connect pointers
        for (size_t i = 0; i < mesh.halfEdge.size(); i++) {
            const auto &data = ControlCage::PyramidCage::HALF_EDGES[i];
            mesh.halfEdge[i].vertex = &mesh.vertex[data.vertex];
            mesh.halfEdge[i].twin = &mesh.halfEdge[data.twin];
            mesh.halfEdge[i].next = &mesh.halfEdge[data.next];
            mesh.halfEdge[i].face = &mesh.faces[data.face];

            // connect vertex entry points
            mesh.halfEdge[i].vertex->outEdge = &mesh.halfEdge[i];
        }

        // connect face entry points
        for (size_t i = 0; i < mesh.faces.size(); i++) {
            mesh.faces[i].halfEdge =
                &mesh.halfEdge[ControlCage::PyramidCage::FACE_START[i]];
        }

        break;
    }
    }

    return mesh;
};
