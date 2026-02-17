#include "render.hpp"
#include <memory>

Render::Render(const std::shared_ptr<Mesh> &shape) {
    this->vertices = shape->surfaceInterpolation(Render::SUBDIVISION);
    this->indices = shape->surfaceGrids(Render::SUBDIVISION);
};
