#ifndef PROJECT_H
#define PROJECT_H

class project {
  public:
    double e[3];

    project(double x, double y, double z) : e{x / z, y / z, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];
};

#endif // !PROJECT_H
