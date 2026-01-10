#ifndef CUBE_H
#define CUBE_H

#include "vect.h"
#include <array>
#include <cstdint>
#include <vector>

class cube {
  public:
    double sides = 0.5;

    void get_vertices() {
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

            vectors.push_back(val);
        }
    };

    void get_edges() {
        // use bit ^ (XOR) operator to flip the values and identify the n
        std::array<int, 2> edge;
        for (int i = 0; i < num_vtx; i++) {
            for (int j = 0; j < val.size(); j++) {
                int n = i ^ (1 << j); // i is "from" and n is "to"

                if (i < n) {
                    edge[0] = i;
                    edge[1] = n;
                }
                faces.push_back(edge);
            }
        }
    }

    std::vector<vect3> vertices() const { return this->vectors; };
    std::vector<std::array<int, 2>> edges() const { return this->faces; };

  private:
    vect3 val;
    const uint32_t num_vtx = 8;

    std::vector<vect3> vectors;
    std::vector<std::array<int, 2>> faces;
};

#endif // !CUBE_H
