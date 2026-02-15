#include "render.hpp"
#include <memory>

Render::Render(const std::shared_ptr<Mesh> &shape) {
    this->vertices = shape->surfaceInterpolation(this->subdivision);
    this->indices = shape->surfaceGrids(this->subdivision);
};
