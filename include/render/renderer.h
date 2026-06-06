#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>
#include <vector>
#include "physics/engine.h"

class Renderer {
public:
    Renderer() = default;
    void draw_world(const engine& phys_engine) const noexcept;

private:
    void draw_smooth_outline(const engine& phys_engine) const noexcept;
    vec2d get_catmull_rom_point(vec2d p0, vec2d p1, vec2d p2, vec2d p3, float t) const noexcept;
};

#endif