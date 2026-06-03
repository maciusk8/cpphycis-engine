#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "spring.h"
#include "vec2d.h"

struct Node {
    vec2d pos;
    vec2d prev_pos;
    vec2d force;
    float mass;
    float radius;
};

struct engine {
    std::vector<Node> nodes;
    std::vector<Spring> springs;
    constexpr static float dt = 0.002f; // 8x mniejszy krok dla ogromnej stabilności!

    // Zmienne ciśnienia (Pressure Soft Body)
    float target_area = 0.0f;
    float pressure_mult = 0.0f;

    float get_volume() const noexcept;
    void step() noexcept;
    void apply_forces() noexcept;
    void integrate() noexcept;
};

#endif