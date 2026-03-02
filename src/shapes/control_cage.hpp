#ifndef CONTROL_CAGE_HPP
#define CONTROL_CAGE_HPP

#pragma once

#include <array>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <sys/types.h>

struct HalfEdgeData {
    const uint16_t vertex;
    const uint16_t twin;
    const uint16_t next;
    const uint16_t face;
};

// predefined cube as guide
struct ControlCage {
    static constexpr std::array<glm::vec3, 8> CAGE_BOUNDARY = {{{-1, -1, -1},
                                                                {1, -1, -1},
                                                                {1, 1, -1},
                                                                {-1, 1, -1},
                                                                {-1, -1, 1},
                                                                {1, -1, 1},
                                                                {1, 1, 1},
                                                                {-1, 1, 1}}};

    static constexpr std::array<std::array<uint16_t, 4>, 6> CAGE_FACES = {{
        {0, 1, 2, 3}, // front
        {5, 4, 7, 6}, // back
        {3, 2, 6, 7}, // top
        {1, 0, 4, 5}, // bot
        {1, 5, 6, 2}, // right
        {3, 7, 4, 0}, // left
    }};

    static constexpr std::array<HalfEdgeData, 24> HALF_EDGES = {{// front face
                                                                 {1, 12, 1, 0},
                                                                 {2, 19, 2, 0},
                                                                 {3, 8, 3, 0},
                                                                 {0, 23, 0, 0},

                                                                 // back face
                                                                 {4, 14, 5, 1},
                                                                 {7, 21, 6, 1},
                                                                 {6, 10, 7, 1},
                                                                 {5, 17, 4, 1},

                                                                 // top face
                                                                 {2, 2, 9, 2},
                                                                 {6, 18, 10, 2},
                                                                 {7, 6, 11, 2},
                                                                 {3, 20, 8, 2},

                                                                 // bot face
                                                                 {0, 0, 13, 3},
                                                                 {4, 22, 14, 3},
                                                                 {5, 4, 15, 3},
                                                                 {1, 16, 12, 3},

                                                                 // right face
                                                                 {5, 15, 17, 4},
                                                                 {6, 7, 18, 4},
                                                                 {2, 9, 19, 4},
                                                                 {1, 1, 16, 4},

                                                                 // left face
                                                                 {7, 11, 21, 5},
                                                                 {4, 5, 22, 5},
                                                                 {0, 13, 23, 5},
                                                                 {3, 3, 20, 5}}};

    static constexpr std::array<uint16_t, 6> FACE_START = {{0, 4, 8, 12, 16, 20}};

    static glm::vec3 bilinear(const std::array<uint16_t, 4> &corners,
                              const float &j, const float &k,
                              const size_t &subdivision) {
        float u = j / subdivision;
        float v = k / subdivision;

        auto p0 = ControlCage::CAGE_BOUNDARY[corners[0]];
        auto p1 = ControlCage::CAGE_BOUNDARY[corners[1]];
        auto p2 = ControlCage::CAGE_BOUNDARY[corners[2]];
        auto p3 = ControlCage::CAGE_BOUNDARY[corners[3]];

        // Bilinear interpolation
        return (1 - u) * (1 - v) * p0 + u * (1 - v) * p1 + u * v * p2 +
               (1 - u) * v * p3;
    };
};

#endif // !CONTROL_CAGE_HPP
