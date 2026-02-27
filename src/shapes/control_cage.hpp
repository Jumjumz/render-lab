#ifndef CONTROL_CAGE_HPP
#define CONTROL_CAGE_HPP

#pragma once

#include <array>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <sys/types.h>

struct HalfEdgeData {
    uint16_t vertex;
    uint16_t twin;
    uint16_t next;
    uint16_t face;
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
        {0, 1, 3, 2}, // front
        {4, 5, 7, 6}, // back
        {2, 3, 6, 7}, // top
        {0, 1, 4, 5}, // bot
        {1, 2, 5, 6}, // right
        {0, 3, 4, 7}, // left
    }};

    static constexpr std::array<HalfEdgeData, 24> HALF_EDGES = {{// front face
                                                                 {1, 20, 1, 0},
                                                                 {2, 12, 2, 0},
                                                                 {3, 23, 3, 0},
                                                                 {0, 8, 0, 0},

                                                                 // back face
                                                                 {5, 16, 5, 1},
                                                                 {6, 15, 6, 1},
                                                                 {7, 13, 7, 1},
                                                                 {4, 21, 4, 1},

                                                                 // top face
                                                                 {2, 3, 2, 2},
                                                                 {6, 22, 10, 2},
                                                                 {7, 6, 13, 2},
                                                                 {3, 5, 3, 2},

                                                                 // bot face
                                                                 {5, 1, 13, 3},
                                                                 {}}};

    static constexpr std::array<uint16_t, 6> FACE_START = {{0, 4, 8, 12, 16, 20}};
};

#endif // !CONTROL_CAGE_HPP
