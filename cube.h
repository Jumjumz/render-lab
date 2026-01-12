#ifndef CUBE_H
#define CUBE_H

#include "vect.h"
#include "vertex.h"
#include <array>
#include <cstdint>
#include <vector>

class cube : public vertex {
  public:
    cube(double sides) : sides(sides) {};

    std::vector<vect3> points() override {
        // use bit & operator to identify the 1.. if a bit is 1 it is value is negative
        for (int i = 0; i < num_vtx; i++) {
            for (int j = 0; j < val.size(); j++) {
                int b = i & (1 << j);

                if (b == (1 << j)) {
                    val.e[j] = -sides / 2;
                } else {
                    val.e[j] = sides / 2;
                }
            }

            vertices.push_back(val);
        }

        return vertices;
    };

    std::vector<std::array<int, 2>> lines() override {
        // use bit ^ (XOR) operator to flip the values and identify the num_vtx
        for (int i = 0; i < num_vtx; i++) {
            std::array<int, 2> edge;
            for (int j = 0; j < val.size(); j++) {
                int n = i ^ (1 << j); // i is "from" and n is "to"

                if (i < n) {
                    edge[0] = i;
                    edge[1] = n;
                }

                edges.push_back(edge);
            }
        }

        return edges;
    }

    std::vector<vect3> between_vertices() override {
        for (int i = 0; i < vertices.size(); i++) {
            std::vector<vect3> u_steps = this->steps(
                i, vertices[i], vertices[(i + 1) % vertices.size()], 10);
            std::vector<vect3> v_steps = this->steps(
                i, vertices[i], vertices[(i + 1) % vertices.size()], 10);

            for (vect3 u : u_steps) {
                for (vect3 v : v_steps) {
                    vll = (1.0 - u) * (1.0 - v) * vertices[i] +
                          u * (1.0 - v) * vertices[(i + 1) % vertices.size()] +
                          u * v * vertices[(i + 2) % vertices.size()] +
                          (1.0 - u) * v * vertices[(i + 3) % vertices.size()];
                }

                points_between_vertices.push_back(vll);
            }
        }

        return points_between_vertices;
        // cubes faces using index..
        // top-face: (1, 2, 5, 6), bot-face: (0, 3, 4, 7), right-face: (0, 1, 4,
        // 5), left: (3, 2, 7, 6), front-face: (4, 5, 6, 7), back-face: (0, 1, 2, 3)
    }

    std::vector<vect3> steps(int index, vect3 &from, vect3 &to, int reso) {
        std::vector<vect3> sts;
        vect3 step = from + (double(index) / reso - 1) * (to - from);

        sts.push_back(step);
        return sts;
    }

  private:
    vect3 val;
    vect3 vll;
    double sides = 0.5;

    const uint32_t num_vtx = 8;

    std::vector<vect3> vertices;
    std::vector<std::array<int, 2>> edges;
    std::vector<vect3> points_between_vertices;
};

#endif // !CUBE_H
