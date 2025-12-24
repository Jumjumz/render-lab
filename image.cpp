#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>
#include <ostream>

int main() {

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // image_height needs to be 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // calculate vectors across the horizontal and down the vertical viewport
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // calculate the horizontal and vertial delta vectors from pixel to pixel
    auto pixel_data_u = viewport_u / image_height;
    auto piexel_data_v = viewport_v / image_height;

    // calculate the location of the upper left pixel
    auto viewport_upper_left = camera_center;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\nScanlines remaining: " << (image_height - j) << ' '
                  << std::flush;

        for (int i = 0; i < image_width; i++) {
            auto pixel_color = color(double(i) / (image_width - 1),
                                     double(j) / (image_height - 1), 0);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\nDone.\n";

    return 0;
}
