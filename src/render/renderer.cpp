#include "render/renderer.h"

// krzywa Catmull-Rom
vec2d Renderer::get_catmull_rom_point(vec2d p0, vec2d p1, vec2d p2, vec2d p3, float t) const noexcept {
    float t2 = t * t;
    float t3 = t2 * t;

    return {
        0.5f * ((2.0f * p1.x) +
                (-p0.x + p2.x) * t +
                (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3),
        0.5f * ((2.0f * p1.y) +
                (-p0.y + p2.y) * t +
                (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3)
    };
}

void Renderer::draw_smooth_outline(const engine& phys_engine) const noexcept {
    const auto& nodes = phys_engine.nodes;
    int n = static_cast<int>(nodes.size());
    if (n < 4) return;

    // rozdzielczosc krzywej
    const int segments = 8; 

    for (int i = 0; i < n; ++i) {
        vec2d p0 = nodes[(i - 1 + n) % n].pos;
        vec2d p1 = nodes[i].pos;
        vec2d p2 = nodes[(i + 1) % n].pos;
        vec2d p3 = nodes[(i + 2) % n].pos;

        vec2d prev_point = p1;

        for (int j = 1; j <= segments; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(segments);
            vec2d current_point = get_catmull_rom_point(p0, p1, p2, p3, t);

            DrawLineEx({prev_point.x, prev_point.y}, {current_point.x, current_point.y}, 6.0f, DARKBLUE);
            prev_point = current_point;
        }
    }
}

void Renderer::draw_world(const engine& phys_engine) const noexcept {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!phys_engine.nodes.empty()) {
        std::vector<Vector2> poly_points;
        vec2d center_pos = {0.0f, 0.0f};
        
        for (const auto& n : phys_engine.nodes) {
            center_pos = center_pos + n.pos;
        }
        center_pos = center_pos * (1.0f / static_cast<float>(phys_engine.nodes.size()));
        
        poly_points.push_back({center_pos.x, center_pos.y});
        
        for (int i = static_cast<int>(phys_engine.nodes.size()) - 1; i >= 0; --i) {
            poly_points.push_back({phys_engine.nodes[i].pos.x, phys_engine.nodes[i].pos.y});
        }
        poly_points.push_back({phys_engine.nodes.back().pos.x, phys_engine.nodes.back().pos.y});
        
        DrawTriangleFan(poly_points.data(), static_cast<int>(poly_points.size()), SKYBLUE);
    }

    draw_smooth_outline(phys_engine);

    for (const auto& node : phys_engine.nodes) {
        DrawCircle(static_cast<int>(node.pos.x), static_cast<int>(node.pos.y), node.radius, DARKBLUE);
    }

    DrawText("Przeciagnij wezly myszka! (Pressure Soft Body)", 10, 10, 20, DARKGRAY);
    EndDrawing();
}