#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"

using camera = point3;

// Can add a camera class for camera manipulation

inline const camera camera_center() { return camera(0, 0, 0); };

inline camera camera_location(double x, double y, double focal_point) {
    return camera(x, y, focal_point);
};

#endif // !CAMERA_H
#define CAMERA_H
