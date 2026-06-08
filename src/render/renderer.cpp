#include "render/renderer.h"

void Renderer::draw_world(const engine& phys_engine, const std::vector<SoftBody>& bodies) const noexcept {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw solid shapes for bodies that have a closed hull
    for (const auto& body : bodies) {
        if (body.type == BodyType::Ball || body.type == BodyType::Balloon) {
            if (body.hull_start < body.hull_end) {
                std::vector<Vector2> poly_points;
                vec2d center_pos = {0.0f, 0.0f};

                for (size_t i = body.hull_start; i < body.hull_end; ++i) {
                    center_pos = center_pos + phys_engine.nodes[i].pos;
                }
                float count = static_cast<float>(body.hull_end - body.hull_start);
                center_pos = center_pos * (1.0f / count);

                poly_points.push_back({center_pos.x, center_pos.y});

                for (int i = static_cast<int>(body.hull_end) - 1; i >= static_cast<int>(body.hull_start); --i) {
                    poly_points.push_back({phys_engine.nodes[i].pos.x, phys_engine.nodes[i].pos.y});
                }
                poly_points.push_back({phys_engine.nodes[body.hull_end - 1].pos.x, phys_engine.nodes[body.hull_end - 1].pos.y});

                DrawTriangleFan(poly_points.data(), static_cast<int>(poly_points.size()), SKYBLUE);
            }
        }
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