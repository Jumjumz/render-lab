#ifndef OBJECTS_H
#define OBJECTS_H

#include "vect.h"
#include <vector>

class cube {
  public:
    double sides = 0.5;

    cube(double &sides) : sides(sides) {};

    void get_vertices() {
        vect3 val;

        for (int i = 0; i < num_vtx; i++) {
            if (i % 2 == 1) {
                for (int j = 0; j < val.size(); j++) {
                    val.e.at(j) = sides / 2;
                }
                vectors.push_back(val);
            } else {
                for (int j = 0; j < val.size(); j++) {
                    val.e.at(j) = -sides / 2;
                }

                vectors.push_back(val);
            }
        }
    };

    std::vector<vect3> vertices() const { return this->vectors; };

  private:
    const int num_vtx = 8;
    std::vector<vect3> vectors;
};

#endif // !OBJECTS_H
