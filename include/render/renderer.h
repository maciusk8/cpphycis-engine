#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>
#include <vector>
#include "physics/engine.h"
#include "physics/softBody.h"

class Renderer {
public:
    Renderer() = default;
    
    void draw_world(const engine& phys_engine, const std::vector<SoftBody>& bodies) const noexcept;

private:
    void draw_smooth_blob(const engine& phys_engine) const noexcept;
    void draw_debug_info(const engine& phys_engine) const noexcept;
};

#endif