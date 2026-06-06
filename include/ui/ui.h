#ifndef UI_H
#define UI_H

#include "physics/engine.h"

class UI {
public:
    UI() = default;

    void init() const noexcept;
    void shutdown() const noexcept;

    void draw(engine& phys_engine) noexcept;

    bool wants_mouse_capture() const noexcept;

private:
    float next_radius = 150.0f;
    int next_num_points = 10;
};

#endif