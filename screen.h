#ifndef SCREEN_H
#define SCREEN_H

#include "vec.h"
#include <cmath>

class screen {
  public:
    float focal_point;
    float angle;
    const float aspect_ratio;
    const int screen_width;
    const int screen_height;

    vec point_position(vec position) {
        vec display = project(position);

        return {((display.x / aspect_ratio) + 1) / 2 * screen_width,
                (1 - (display.y + 1) / 2 * screen_height), position.z};
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

inline vec display(double x, double y, float &aspect_ratio, int &screen_width,
                   int &screen_height) {
    // -1.. 1 => 0..2 => 0..1 <- by dividing to 2
    return {((x / aspect_ratio) + 1) / 2 * screen_width,
            (1 - (y + 1) / 2) * screen_height}; // 1- as y axis is flipped
}

#endif // !SCREEN_H
