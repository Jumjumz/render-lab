#ifndef SCREEN_H
#define SCREEN_H

class screen {
  public:
    double e[2];

    screen(double x, double y, int &screen_width, int &screen_height)
        : e{(x + 1) / 2 * screen_width, (1 - (y + 1) / 2) * screen_height} {
          }; // 1- as the y axis is flip and needs to inverse

    const double x = this->e[0];
    const double y = this->e[1];
};

inline int offset(double axis, int &size) { return axis - double(size) / 2; }

#endif // !SCREEN_H
