#ifndef OBJECTS_H
#define OBJECTS_H

#include "vect.h"
#include <array>
#include <cstdint>
#include <vector>

class cube {
  public:
    double sides = 0.5;

    void get_vertices() {
        vect3 val;

        // use bit & operator to identify the 1.. if bit is 1 it is value is negative
        for (int i = 0; i < num_vtx; i++) {
            if ((i & 1) == 1) {
                val.e[0] = -sides / 2;
            }

            if ((i & 1) == 0) {
                val.e[0] = sides / 2;
            }

            if ((i & 2) == 2) {
                val.e[1] = -sides / 2;
            }

            if ((i & 2) == 0) {
                val.e[1] = sides / 2;
            }

            if ((i & 4) == 4) {
                val.e[2] = -sides / 2;
            }

            if ((i & 4) == 0) {
                val.e[2] = sides / 2;
            }

            vectors.push_back(val);
        }
    };

    void get_edges() {
        std::array<int, 2> edge;
        for (int i = 0; i < num_vtx; i++) {
            for (int j = 0; j < 3; j++) {
                int n = i ^ (1 << j);

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
    const uint32_t num_vtx = 8;
    const uint32_t num_edg = 12;
    std::vector<vect3> vectors;
    std::vector<std::array<int, 2>> faces;
};

#endif // !OBJECTS_H
