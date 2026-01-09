#ifndef OBJECTS_H
#define OBJECTS_H

#include "vect.h"
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
        for (int i = 0; i < num_vtx; i++) {
        }
    }

    std::vector<vect3> vertices() const { return this->vectors; };
    std::vector<std::vector<int>> edges() const { return this->faces; };

  private:
    const uint32_t num_vtx = 8;
    std::vector<vect3> vectors;
    std::vector<std::vector<int>> faces;
};

#endif // !OBJECTS_H
