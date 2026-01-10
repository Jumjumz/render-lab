#ifndef CUBE_H
#define CUBE_H

#include "vect.h"
#include "vertex.h"
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

            vectors.push_back(val);
        }

        return vectors;
    };

    std::vector<std::vector<int>> lines() override {
        // use bit ^ (XOR) operator to flip the values and identify the num_vtx
        for (int i = 0; i < num_vtx; i++) {
            std::vector<int> edge;
            for (int j = 0; j < val.size(); j++) {
                int n = i ^ (1 << j); // i is "from" and n is "to"

                if (i < n) {
                    edge.push_back(i);
                    edge.push_back(n);
                }

                faces.push_back(edge);
            }
        }

        return faces;
    }

    std::vector<vect3> vertices() const { return this->vectors; };
    std::vector<std::vector<int>> edges() const { return this->faces; };

  private:
    vect3 val;
    double sides = 0.5;

    const uint32_t num_vtx = 8;

    std::vector<vect3> vectors;
    std::vector<std::vector<int>> faces;
};

#endif // !CUBE_H
