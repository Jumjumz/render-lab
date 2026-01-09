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

        // use bit & operator to identify the 1.. if a bit is 1 it is value is negative
        for (int i = 0; i < num_vtx; i++) {
            for (int j = 0; j < val.size(); j++) {
                int n = i & (1 << j);

                if (n == (1 << j)) {
                    val.e[j] = -sides / 2;
                } else {
                    val.e[j] = sides / 2;
                }
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
    std::vector<vect3> vectors;
    std::vector<std::array<int, 2>> faces;
};

#endif // !OBJECTS_H
