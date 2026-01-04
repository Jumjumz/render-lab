#ifndef SCREEN_H
#define SCREEN_H

class screen {
  public:
    double e[3];

    screen() : e{e[0], e[1]} {};

    screen(double x, double y) : e{x, y} {};

    screen(double x, double y, double z) : e{x, y, z} {};

    const double x = this->e[0];
    const double y = this->e[1];
    const double z = this->e[2];
};

inline screen project(double x, double y, double z) {
    return screen(x / z, y / z);
}

inline screen display(double x, double y, int &screen_width, int &screen_height) {
    return screen((x + 1) / 2 * screen_width,
                  (1 - (y + 1) / 2) * screen_height); // 1- as y axis is flipped
}

inline int offset(double axis, int &size) { return axis - double(size) / 2; }

#endif // !SCREEN_H
