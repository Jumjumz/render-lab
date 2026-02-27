#ifndef HALF_EDGE_HPP
#define HALF_EDGE_HPP

#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

struct HalfEdge;

struct HalfEdgeVertex {
    glm::vec3 position;
    HalfEdge *outEdge;
};

struct Face {
    HalfEdge *halfEdge;
};

struct HalfEdge {
    HalfEdgeVertex *vertex;
    HalfEdge *twin;
    HalfEdge *next;
    Face *face;
};

#endif // !HALF_EDGE_HPP
