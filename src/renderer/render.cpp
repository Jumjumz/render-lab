#include "render.hpp"
#include "shapes/cube.hpp"
#include "shapes/mesh.hpp"
#include "shapes/pyramid.hpp"
#include "shapes/sphere.hpp"
#include <memory>

Render::Render() { this->renderData = renderShape(Shapes::CUBE); };

Render::RenderData Render::renderShape(const Shapes &render) {
    auto shape = std::shared_ptr<Mesh>();

    switch (render) {
    case Shapes::CUBE: {
        shape = std::make_shared<Cube>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
        };
    }

    case Shapes::SPHERE: {
        shape = std::make_shared<Sphere>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
        };
    }

    case Shapes::PYRAMID: {
        shape = std::make_shared<Pyramid>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
        };
    }

    default: {
        shape = std::make_shared<Cube>(Render::SIZE);

        return RenderData{
            shape->surfaceInterpolation(Render::SUBDIVISION),
            shape->surfaceGrids(Render::SUBDIVISION),
        };
    }
    }
};
