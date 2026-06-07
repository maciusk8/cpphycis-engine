#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>
#include <vector>
#include "physics/engine.h"

class Renderer {
public:
    Renderer() = default;
    void draw_world(const engine& phys_engine, int curve_segments) const noexcept;

private:
    void draw_smooth_outline(const engine& phys_engine, int curve_segments) const noexcept;
    vec2d get_catmull_rom_point(vec2d p0, vec2d p1, vec2d p2, vec2d p3, float t) const noexcept;

    void draw_filled_blob(const engine& phys_engine) const noexcept;
    float cross_product(vec2d a, vec2d b, vec2d c) const noexcept;
    bool is_point_in_triangle(vec2d p, vec2d a, vec2d b, vec2d c) const noexcept;
};

#endif