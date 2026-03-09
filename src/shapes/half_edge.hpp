#ifndef HALF_EDGE_HPP
#define HALF_EDGE_HPP

#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

struct HalfEdge;

struct HalfEdgeVertex {
    glm::vec3 position;
    HalfEdge *outEdge = nullptr;
};

struct Face {
    HalfEdge *halfEdge = nullptr;
    std::vector<glm::vec3> vertices;
};

struct HalfEdge {
    HalfEdgeVertex *vertex = nullptr;
    HalfEdge *twin = nullptr;
    HalfEdge *next = nullptr;
    Face *face = nullptr;
};

#endif // !HALF_EDGE_HPP
