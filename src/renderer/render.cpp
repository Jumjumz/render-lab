#include "render.hpp"
#include "shapes/cube.hpp"
#include "shapes/pyramid.hpp"
#include "shapes/sphere.hpp"
#include <memory>

Render::Render() { this->renderData = renderShape(Shapes::CUBE); };

RenderData Render::renderShape(const Shapes &render) const {
    switch (render) {
    case Shapes::CUBE: {
        auto shape = std::make_unique<Cube>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
            true,
        };
    }

    case Shapes::SPHERE: {
        auto shape = std::make_unique<Sphere>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
            true,
        };
    }

    case Shapes::PYRAMID: {
        auto shape = std::make_unique<Pyramid>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
            true,
        };
    }

    default: {
        auto shape = std::make_unique<Cube>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
            true,
        };
    }
    }
};
