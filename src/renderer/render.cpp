#include "render.hpp"
#include "shapes/cube.hpp"
#include "shapes/cylinder.hpp"
#include "shapes/pyramid.hpp"
#include "shapes/sphere.hpp"
#include <memory>

Render::Render() { this->renderData = renderShape(Shapes::CUBE); };

RenderData Render::renderShape(const Shapes &render) const {
    switch (render) {
    case Shapes::CUBE: {
        auto shape = std::make_unique<Cube>(Render::SIZE);

        return RenderData{
            shape->surface(Render::SUBDIVISION).vertices,
            shape->surface(Render::SUBDIVISION).indices,
            true,
        };
    }

    case Shapes::SPHERE: {
        auto shape = std::make_unique<Sphere>(Render::SIZE);

        return RenderData{
            shape->surface(Render::SUBDIVISION).vertices,
            shape->surface(Render::SUBDIVISION).indices,
            true,
        };
    }

    case Shapes::CYLINDER: {
        auto shape = std::make_unique<Cylinder>(Render::SIZE);

        return RenderData{shape->surface(Render::SUBDIVISION).vertices,
                          shape->surface(Render::SUBDIVISION).indices, true};
    }

    case Shapes::PYRAMID: {
        // update to pyramid
        auto shape = std::make_unique<Pyramid>(Render::SIZE);

        return RenderData{
            shape->surface(Render::SUBDIVISION).vertices,
            shape->surface(Render::SUBDIVISION).indices,
            true,
        };
    }

    default: {
        auto shape = std::make_unique<Cube>(Render::SIZE);

        return RenderData{
            shape->surface(Render::SUBDIVISION).vertices,
            shape->surface(Render::SUBDIVISION).indices,
            true,
        };
    }
    }
};
