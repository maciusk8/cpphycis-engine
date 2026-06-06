#include "render/renderer.h"

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

    for (const auto& spring : phys_engine.springs) {
        vec2d pA = phys_engine.nodes[spring.idA].pos;
        vec2d pB = phys_engine.nodes[spring.idB].pos;
        DrawLineEx({pA.x, pA.y}, {pB.x, pB.y}, 5.0f, BLUE);
    }

    for (const auto& node : phys_engine.nodes) {
        DrawCircle(static_cast<int>(node.pos.x), static_cast<int>(node.pos.y), node.radius, DARKBLUE);
    }

    DrawText("Przeciagnij wezly myszka! (Pressure Soft Body)", 10, 10, 20, DARKGRAY);
    EndDrawing();
}