#include "render.hpp"
#include <memory>

Render::Render(const std::shared_ptr<Mesh> &shape) {
    this->vertices = shape->surfaceInterpolation(Render::subdivision);
    this->indices = shape->surfaceGrids(Render::subdivision);
};
