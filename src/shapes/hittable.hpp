#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#pragma once

#include <glm/fwd.hpp>
#include <vector>

class Hittable {
  public:
    virtual ~Hittable() = default;

    virtual std::vector<glm::vec3> points() = 0;

  private:
};

#endif // !HITTABLE_HPP
