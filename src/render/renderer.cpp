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

float Renderer::cross_product(vec2d a, vec2d b, vec2d c) const noexcept {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool Renderer::is_point_in_triangle(vec2d p, vec2d a, vec2d b, vec2d c) const noexcept {
    float w1 = cross_product(a, b, p);
    float w2 = cross_product(b, c, p);
    float w3 = cross_product(c, a, p);
    
    return (w1 > 0.0f && w2 > 0.0f && w3 > 0.0f) || 
           (w1 < 0.0f && w2 < 0.0f && w3 < 0.0f);
}

void Renderer::draw_filled_blob(const engine& phys_engine) const noexcept {
    std::vector<vec2d> poly;
    
    // CCW
    for (int i = static_cast<int>(phys_engine.nodes.size()) - 1; i >= 0; --i) {
        poly.push_back(phys_engine.nodes[i].pos);
    }

    if (poly.size() < 3) return;

    std::vector<vec2d> remaining = poly;

    while (remaining.size() > 2) {
        bool ear_found = false;
        int n = static_cast<int>(remaining.size());

        for (int i = 0; i < n; ++i) {
            int prev = (i - 1 + n) % n;
            int next = (i + 1) % n;

            vec2d a = remaining[prev];
            vec2d b = remaining[i];
            vec2d c = remaining[next];

            // czy wypukly
            if (cross_product(a, b, c) < 0.0f) {
                bool is_ear = true;
                
                // czy nie ma overlapu
                for (int j = 0; j < n; ++j) {
                    if (j == prev || j == i || j == next) continue;
                    if (is_point_in_triangle(remaining[j], a, b, c)) {
                        is_ear = false;
                        break;
                    }
                }

                if (is_ear) {
                    DrawTriangle({a.x, a.y}, {b.x, b.y}, {c.x, c.y}, SKYBLUE);
                    remaining.erase(remaining.begin() + i);
                    ear_found = true;
                    break;
                }
            }
        }
        if (!ear_found) break; 
    }

    if (remaining.size() >= 3) {
        for (size_t i = 1; i < remaining.size() - 1; ++i) {
            DrawTriangle(
                {remaining[0].x, remaining[0].y}, 
                {remaining[i].x, remaining[i].y}, 
                {remaining[i+1].x, remaining[i+1].y}, 
                SKYBLUE
            );
        }
    }
}

void Renderer::draw_smooth_outline(const engine& phys_engine, int curve_segments) const noexcept {
    const auto& nodes = phys_engine.nodes;
    int n = static_cast<int>(nodes.size());
    if (n < 4) return;

for (int i = 0; i < n; ++i) {
        vec2d p0 = nodes[(i - 1 + n) % n].pos;
        vec2d p1 = nodes[i].pos;
        vec2d p2 = nodes[(i + 1) % n].pos;
        vec2d p3 = nodes[(i + 2) % n].pos;

        vec2d prev_point = p1;

        for (int j = 1; j <= curve_segments; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(curve_segments);
            vec2d current_point = get_catmull_rom_point(p0, p1, p2, p3, t);

            DrawTriangle({p1.x, p1.y}, {current_point.x, current_point.y}, {prev_point.x, prev_point.y}, SKYBLUE);
            DrawTriangle({p1.x, p1.y}, {prev_point.x, prev_point.y}, {current_point.x, current_point.y}, SKYBLUE);

            prev_point = current_point;
        }
    }

    for (int i = 0; i < n; ++i) {
        vec2d p0 = nodes[(i - 1 + n) % n].pos;
        vec2d p1 = nodes[i].pos;
        vec2d p2 = nodes[(i + 1) % n].pos;
        vec2d p3 = nodes[(i + 2) % n].pos;

        vec2d prev_point = p1;

        for (int j = 1; j <= curve_segments; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(curve_segments);
            vec2d current_point = get_catmull_rom_point(p0, p1, p2, p3, t);

            DrawLineEx({prev_point.x, prev_point.y}, {current_point.x, current_point.y}, 6.0f, DARKBLUE);
            prev_point = current_point;
        }
    }
}

void Renderer::draw_world(const engine& phys_engine, int curve_segments) const noexcept {
    draw_filled_blob(phys_engine);
    draw_smooth_outline(phys_engine, curve_segments);

    for (const auto& node : phys_engine.nodes) {
        DrawCircle(static_cast<int>(node.pos.x), static_cast<int>(node.pos.y), node.radius, DARKBLUE);
    }

    DrawText("Przeciagnij wezly myszka! (Pressure Soft Body)", 10, 10, 20, DARKGRAY);
}