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
    float dt = 0.002f; // 8x mniejszy krok dla ogromnej stabilności!

    // Zmienne ciśnienia (Pressure Soft Body)
    vec2d gravity = {0.0f, 1000.0f};
    float target_area = 0.0f;
    float pressure_mult = 0.0f;

    float get_volume() const noexcept;
    void step() noexcept;
    void apply_forces() noexcept;
    void apply_pressure(const std::vector<SoftBody>& bodies) noexcept;
    void integrate() noexcept;
    void apply_collisions() noexcept;
};

#endif