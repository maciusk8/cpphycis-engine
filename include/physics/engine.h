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
    SimColor color;
};

#include "physics/grid.h"
struct engine {
    std::vector<Node> nodes;
    std::vector<Spring> springs;
    float dt = 0.002f; // 8x mniejszy krok dla ogromnej stabilności!
    float node_radius = 12.0f;
    float collision_stiffness = 1.0f;

    // Zmienne ciśnienia (Pressure Soft Body)
    vec2d gravity = {0.0f, 1000.0f};

    SpatialGrid grid{1200.0f, 800.0f, 40.0f};

    float get_area(size_t start, size_t end) const noexcept;
    void step(const std::vector<SoftBody>& bodies) noexcept;
    void apply_forces() noexcept;
    void apply_pressure(const std::vector<SoftBody>& bodies) noexcept;
    void integrate() noexcept;
    void apply_collisions() noexcept;
};

#endif