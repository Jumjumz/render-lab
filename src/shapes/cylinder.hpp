#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#pragma once

#include "hittable.hpp"

class Cylinder : public Hittable {
  public:
    Cylinder();

    std::vector<glm::vec3> points() override;

  private:
};

#endif // !CYLINDER_HPP
