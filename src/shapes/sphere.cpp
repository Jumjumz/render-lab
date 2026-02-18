#include "sphere.hpp"

Sphere::Sphere(const float &radius) : radius(radius) {};

std::vector<Vertex> Sphere::surfaceInterpolation(const size_t &subdivision) {
    size_t numPoints = subdivision * Sphere::MULTIPLIER;

    std::vector<Vertex> points;
    glm::vec3 node;

    // fibonacci sphere
    for (size_t i = 0; i < numPoints; i++) {
        node.y = 1 - (float(i) / (numPoints - 1)) * 2;

        float radiusAtY = std::sqrt(1 - (node.y * node.y));
        float theta = (glm::radians(180.0f * 2) / (this->phi * this->phi)) * i;

        node.x = std::cos(theta) * radiusAtY;
        node.z = std::sin(theta) * radiusAtY;

        node *= this->radius;

        points.push_back({node, Sphere::COLOR});
    }

    this->points = points;

    return points;
};

std::vector<uint16_t> Sphere::surfaceGrids(const size_t &subdivision) {
    std::vector<uint16_t> arcs;

    float dThreshold =
        1.5 *
        std::sqrt((glm::radians(180.0f * 4) * (this->radius * this->radius)) /
                  this->points.size());

    for (size_t i = 0; i < this->points.size(); i++) {
        for (size_t j = i + 1; j < this->points.size(); j++) {
            glm::vec3 diff = this->points[j].pos - this->points[i].pos;
            double dist =
                std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            if (dist < dThreshold) {
                arcs.push_back(i);
                arcs.push_back(j);
            }
        }
    };

    return arcs;
};
