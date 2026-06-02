#include "engine.h"
void engine::apply_forces() noexcept {
    for (const auto& spring : springs) {
        vec2d vec = positions[spring.idA] - positions[spring.idB];
        float x = spring.resting_length - vec.length();          // F = kx
        vec2d force = vec.normalize() * (x * spring.stiffness);  // F = ma
        forces[spring.idA] += force;
        forces[spring.idB] -= force;
    }
}
// calkowanie eulera, niby niestabilne ale narazie starczy
void engine::integrate() noexcept {
    for (size_t i = 0; i < positions.size(); ++i) {
        velocities[i] += forces[i] * dt;
        velocities[i] = velocities[i] * 0.98f;  // tlumienie
        positions[i] += velocities[i] * dt;
        forces[i] = {0.0f, 0.0f};  // prepare for nextFrame
    }
}
void engine::step() noexcept {
    apply_forces();
    integrate();
}