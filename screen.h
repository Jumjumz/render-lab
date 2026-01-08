#ifndef SCREEN_H
#define SCREEN_H

#include "vect.h"
#include <cmath>
#include <cstdint>

class screen {
  public:
    screen(float &focal_point, float &angle, float &aspect_ratio,
           uint32_t &screen_width, uint32_t &screen_height)
        : focal_point(focal_point), angle(angle), aspect_ratio(aspect_ratio),
          screen_width(screen_width), screen_height(screen_height) {};

    vect2 position(vect3 &position) const {
        vect3 display = project(position);
        // -1..1 => 0..2 => / 2 => 0..1 => x / aspect_ratio
        return {((display.x / aspect_ratio) + 1) / 2 * screen_width,
                (1 - (display.y + 1) / 2) * screen_height};
    }

  private:
    const uint32_t screen_width;
    const uint32_t screen_height;

    const float angle;
    const float focal_point;
    const float aspect_ratio;

    vect2 project(vect3 position) const {
        vect3 project = translate_zx(rotate(position));

        return {project.x / project.z, project.y / project.z};
    }

    vect3 translate_zx(vect3 position) const {
        return {position.x, position.y, position.z + focal_point};
    }

    vect3 rotate(vect3 position) const {
        float cos = std::cos(angle);
        float sin = std::sin(angle);

        return {position.x * cos - position.z * sin, position.y,
                position.x * sin + position.z * cos};
    }
};

#endif // !SCREEN_H
