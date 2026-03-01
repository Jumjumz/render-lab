#include "cube.hpp"
#include "control_cage.hpp"

Cube::Cube(const float &sides) : sides(sides) {
    for (auto v : ControlCage::CAGE_BOUNDARY) {
        this->vertices.push_back(v);
    }

    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        size_t h = ControlCage::FACE_START[i];
        size_t start = h;

        std::vector<uint16_t> faceVerts;
        do {
            faceVerts.push_back(ControlCage::HALF_EDGES[h].vertex);

            h = ControlCage::HALF_EDGES[h].next;
        } while (h != start);

        // get the num of vertices on a face
        size_t numFaces = ControlCage::CAGE_FACES[0].size();
        for (size_t j = 0; j < numFaces; j++) {
            this->indices.push_back(faceVerts[j]);
            this->indices.push_back(faceVerts[(j + 1) % numFaces]);
        }
    }
};

std::vector<Vertex> Cube::surfaceInterpolation(const size_t &subdivision) {
    std::vector<Vertex> surfacePoints;

    for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        std::array<uint16_t, 4> corners = ControlCage::CAGE_FACES[i];

        // interpolate
        for (size_t j = 0; j <= subdivision; j++) {
            for (size_t k = 0; k <= subdivision; k++) {
                float u = static_cast<float>(j) / subdivision;
                float v = static_cast<float>(k) / subdivision;

                auto p0 = this->vertices[corners[0]];
                auto p1 = this->vertices[corners[1]];
                auto p2 = this->vertices[corners[2]];
                auto p3 = this->vertices[corners[3]];

                // Bilinear interpolation
                glm::vec3 pt = (1 - u) * (1 - v) * p0 + u * (1 - v) * p1 +
                               u * v * p2 + (1 - u) * v * p3;

                pt *= this->sides;

                surfacePoints.push_back({pt, Cube::COLOR});
            }
        }
    }

    return surfacePoints;
};

std::vector<uint16_t> Cube::surfaceGrids(const size_t &subdivision) {
    /*for (size_t i = 0; i < ControlCage::CAGE_FACES.size(); i++) {
        // build indices
        uint16_t n = subdivision + 1;
        for (size_t j = 0; j < subdivision; j++) {
            for (size_t k = 0; k < subdivision; k++) {
                auto bI = this->vertices.size();
                uint16_t v0 = bI + j * n + k;
                uint16_t v1 = bI + j * n + k + 1;
                uint16_t v2 = bI + (j + 1) * n + k;
                uint16_t v3 = bI + (j + 1) * n + k + 1;

                this->indices.push_back(v0);
                this->indices.push_back(v1);
                this->indices.push_back(v2);
                this->indices.push_back(v3);
            }
        }
    }*/

    return this->indices;
};
