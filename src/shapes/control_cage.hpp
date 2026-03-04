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
    struct CubeCage {
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

        static constexpr std::array<HalfEdgeData, 24> HALF_EDGES = {
            {// front face
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

        static constexpr std::array<uint16_t, 6> FACE_START = {
            {0, 4, 8, 12, 16, 20}};
    };

    struct PyramidCage {
        static constexpr std::array<glm::vec3, 5> CAGE_BOUNDARY = {
            {{0, 1, 0}, {-1, 0, -1}, {1, 0, -1}, {1, 0, 1}, {-1, 0, 1}}};

        static constexpr std::array<std::array<uint16_t, 3>, 6> CAGE_FACES = {{
            {0, 2, 1}, // front
            {0, 3, 2}, // right
            {0, 4, 3}, // back
            {0, 1, 4}, // left
            {4, 3, 1}, // base upper side
            {2, 3, 1}, // base lower side
        }};

        static constexpr std::array<HalfEdgeData, 18> HALF_EDGES = {{
            // front
            {2, 15, 2, 0},
            {1, 11, 1, 0},
            {0, 3, 0, 0},

            // right
            {3, 16, 5, 1},
            {2, 2, 4, 1},
            {0, 6, 3, 1},

            // back
            {4, 13, 8, 2},
            {3, 5, 7, 2},
            {0, 9, 6, 2},

            // left
            {1, 14, 11, 3},
            {4, 8, 10, 3},
            {0, 0, 9, 3},

            // base upper
            {4, 7, 14, 4},
            {3, 17, 13, 4},
            {1, 10, 12, 4},

            // base lower
            {3, 4, 17, 5},
            {2, 1, 16, 5},
            {1, 12, 15, 5},
        }};

        static constexpr std::array<uint16_t, 6> FACE_START = {
            {0, 3, 6, 9, 12, 15}};
    };
};

struct Geometry {
    // for quads
    static glm::vec3 bilinear(const std::array<uint16_t, 4> &corners,
                              const size_t &column, const size_t &row,
                              const size_t &subdivision) {
        float u = (float)column / subdivision;
        float v = (float)row / subdivision;

        auto apex = ControlCage::CubeCage::CAGE_BOUNDARY[corners[0]];
        auto c1 = ControlCage::CubeCage::CAGE_BOUNDARY[corners[1]];
        auto c2 = ControlCage::CubeCage::CAGE_BOUNDARY[corners[2]];
        auto p3 = ControlCage::CubeCage::CAGE_BOUNDARY[corners[3]];

        // Bilinear interpolation
        return (1 - u) * (1 - v) * apex + u * (1 - v) * c1 + u * v * c2 +
               (1 - u) * v * p3;
    };

    // for triangles
    static glm::vec3 barycentric(const std::array<uint16_t, 3> &corners,
                                 const size_t &column, const size_t &row,
                                 const size_t &subdivision) {
        float u = (float)column / subdivision;
        float v = (float)row / subdivision;

        auto apex = ControlCage::PyramidCage::CAGE_BOUNDARY[corners[0]];
        auto c1 = ControlCage::PyramidCage::CAGE_BOUNDARY[corners[1]];
        auto c2 = ControlCage::PyramidCage::CAGE_BOUNDARY[corners[2]];

        return (1.0f - u - v) * apex + (u * c1) + (v * c2);
    };
};

#endif // !CONTROL_CAGE_HPP
