#ifndef MESH_HPP
#define MESH_HPP

#pragma once

#include "control_cage.hpp"
#include "half_edge.hpp"
#include "renderer/vertex.hpp"

#include <cstddef>
#include <glm/fwd.hpp>
#include <sys/types.h>
#include <vector>

struct MeshData;

class Mesh {
  public:
    virtual ~Mesh() = default;

    virtual MeshData surface(const size_t &subdivision) const = 0;

    static constexpr glm::vec3 COLOR = {0.0f, 1.0f, 0.0f};

  protected:
    HalfEdgeData halfEdgeData;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

template <typename CageType>
static MeshData GENERATE_SURFACE(const CageType &cage, const size_t &subdivision) {
    MeshData mesh;

    const size_t cageSize = cage.faces.size();

    for (size_t i = 0; i < cageSize; i++) {
        HalfEdge *h = cage.faces[i].halfEdge;
        const auto n = subdivision + 1;

        std::vector<glm::vec3> corners;
        do {
            corners.push_back(h->vertex->position);
            h = h->next;
        } while (h != cage.faces[i].halfEdge);

        if (corners.size() == 4) {
            const auto faceOffset = i * (n * n);

            for (size_t j = 0; j < n; j++) {
                for (size_t k = 0; k < n; k++) {
                    auto pt = Geometry::bilinear(corners, j, k, subdivision);

                    mesh.vertices.push_back({pt, Mesh::COLOR});

                    const auto current = faceOffset + (j * n) + k;

                    // connect to right
                    if (k < subdivision) {
                        mesh.indices.push_back(current);
                        mesh.indices.push_back(current + 1);
                    }

                    // connect to bottom
                    if (j < subdivision) {
                        mesh.indices.push_back(current);
                        mesh.indices.push_back(current + n);
                    }
                }
            }
        } else if (corners.size() == 3) {
            // interpolate and get grid connection
            for (size_t j = 0; j < n; j++) {
                const auto rowWidth = n - j;
                for (size_t k = 0; k < rowWidth; k++) {
                    auto pt = Geometry::barycentric(corners, j, k, subdivision);

                    mesh.vertices.push_back({pt, Mesh::COLOR});

                    // current sequential index
                    const auto current = mesh.vertices.size() - 1;

                    // connect to right
                    if (k < rowWidth - 1) {
                        mesh.indices.push_back(current);
                        mesh.indices.push_back(current + 1);
                    }

                    // connect down-right diagonally
                    if (j < subdivision && k < rowWidth - 1) {
                        mesh.indices.push_back(current);
                        mesh.indices.push_back(current + rowWidth);
                    }

                    // connect down-left diagonally
                    if (j < subdivision) {
                        mesh.indices.push_back(current + 1);
                        mesh.indices.push_back(current + rowWidth);
                    }
                }
            }
        }
    }

    return mesh;
};

#endif // !MESH_HPP
