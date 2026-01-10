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

  private:
    vect3 val;
    double sides = 0.5;

    const uint32_t num_vtx = 8;

    std::vector<vect3> vertices;
    std::vector<std::array<int, 2>> edges;
};

#endif // !CUBE_H
