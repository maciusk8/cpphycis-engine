#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "spring.h"
#include "vec2d.h"

struct engine {
    // points
    std::vector<vec2d> positions;
    std::vector<vec2d> velocities;
    std::vector<vec2d> forces;
    // springs
    std::vector<Spring> springs;
    constexpr static float dt = 0.016f;

    void step() noexcept;
    void apply_forces() noexcept;
    void integrate() noexcept;
};

#endif