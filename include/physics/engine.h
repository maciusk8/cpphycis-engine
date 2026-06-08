#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "physics/softBody.h"
#include "physics/spring.h"
#include "physics/vec2d.h"

struct Node {
    vec2d pos;
    vec2d prev_pos;
    vec2d force;
    float mass;
    float radius;
};

#include "physics/grid.h"
struct engine {
    std::vector<Node> nodes;
    std::vector<Spring> springs;
    constexpr static float dt = 0.002f;  // 8x mniejszy krok dla ogromnej stabilności!
    SpatialGrid grid{1200.0f, 800.0f, 12.0f};

    float get_area(size_t start, size_t end) const noexcept;
    void step(const std::vector<SoftBody>& bodies) noexcept;
    void apply_forces() noexcept;
    void apply_pressure(const std::vector<SoftBody>& bodies) noexcept;
    void integrate() noexcept;
    void apply_collisions() noexcept;
};

#endif