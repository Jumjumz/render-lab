#ifndef SCREEN_H
#define SCREEN_H

#include "vec.h"
#include <cmath>

class screen {
  public:
    const float angle;
    const int screen_width;
    const int screen_height;
    const float focal_point;
    const float aspect_ratio;

    screen(float &focal_point, float &angle, float &aspect_ratio,
           int &screen_width, int &screen_height)
        : focal_point(focal_point), angle(angle), aspect_ratio(aspect_ratio),
          screen_width(screen_width), screen_height(screen_height) {};

    vec position(vec position) const {
        vec display = project(position);

        return {((display.x / aspect_ratio) + 1) / 2 * screen_width,
                (1 - (display.y + 1) / 2) * screen_height};
    }

  private:
    vec project(vec position) const {
        vec project = translate_zx(rotate(position));

        return {project.x / project.z, project.y / project.z};
    }

    vec translate_zx(vec position) const {
        return {position.x, position.y, position.z + focal_point};
    }

    vec rotate(vec position) const {
        float cos = std::cos(angle);
        float sin = std::sin(angle);

        return {position.x * cos - position.z * sin, position.y,
                position.x * sin + position.z * cos};
    }
};

#endif // !SCREEN_H
