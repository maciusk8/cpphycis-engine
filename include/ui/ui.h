#ifndef UI_H
#define UI_H

#include "physics/engine.h"
#include "physics/softBody.h"
#include <vector>

class UI {
public:
    UI() = default;

    void init() const noexcept;
    void shutdown() const noexcept;
    void draw(engine& phys_engine, std::vector<SoftBody>& bodies) noexcept;
    bool wants_mouse_capture() const noexcept;

    int get_curve_segments() const noexcept { return curve_segments; }
    float get_time_scale() const noexcept { return time_scale; }

private:
    int selected_body_type = 0;
    int spawn_count = 1;

    int target_fps = 240;
    int curve_segments = 8;
    float time_scale = 1.0f;
};

#endif