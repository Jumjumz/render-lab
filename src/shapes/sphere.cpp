#include "sphere.hpp"
#include <vector>

Sphere::Sphere(const float &diameter) { this->radius = diameter / 2; };

std::vector<Vertex> Sphere::surfaceInterpolation(const size_t &subdivision) {
    size_t num_points = subdivision * Sphere::MULTIPLIER;

    std::vector<Vertex> points;
    glm::vec3 val;

    // fibonacci sphere
    for (size_t i = 0; i < num_points; i++) {
        val[1] = this->radius - ((2.0 * i) / num_points - this->radius);

        float radius_at_y = std::sqrt((this->radius - (val[1] * val[1])));
        float theta = glm::radians(360.0f) * (i / this->phi);

        val[0] = std::cos(theta) * radius_at_y;
        val[2] = std::sin(theta) * radius_at_y;

        points.push_back({val, Sphere::COLOR});
    }

    this->points = points;

    return points;
};

std::vector<uint16_t> Sphere::surfaceGrids(const size_t &subdivision) {
    float avg_threshold =
        std::sqrt((glm::radians(360.0f * 4) * this->radius * this->radius) /
                  this->points.size());
    float d_threshold = avg_threshold * (subdivision - 3); // threshold
    std::vector<uint16_t> arcs;

    for (size_t i = 0; i < this->points.size(); i++) {
        for (size_t j = i + 1; j < this->points.size(); j++) {
            glm::vec3 diff = this->points[j].pos - this->points[i].pos;
            double dist =
                std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            if (dist < d_threshold) {
                arcs.push_back(i);
                arcs.push_back(j);
            }
        }
    };

    return arcs;
};
