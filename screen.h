#ifndef SCREEN_H
#define SCREEN_H

class screen {
  public:
    double e[2];

    screen(double x, double y, int screen_width, int screen_height)
        : e{(x + 1) / 2 * screen_width, (y + 1) / 2 * screen_height} {};

    int x = e[0];
    int y = e[1];
};

#endif // !SCREEN_H
