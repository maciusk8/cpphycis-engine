#include <raylib.h>
#include <cmath>
#include <vector>
#include "engine.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Test Silnika Fizycznego");
    SetTargetFPS(60);

    engine my_engine;

    // --- GENEROWANIE BALONU (30 punktów) ---
    const int num_points = 30;
    const float radius = 100.0f;
    const Vector2 center = {400.0f, 300.0f};

    for (int i = 0; i < num_points; ++i) {
        float angle = i * (2.0f * 3.14159f / num_points);

        vec2d pos = {center.x + std::cos(angle) * radius,
                     center.y + std::sin(angle) * radius};
        
        my_engine.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
    }

    // --- ŁĄCZENIE SPRĘŻYN (Tylko na obwodzie) ---
    for (int i = 0; i < num_points; ++i) {
        int next_i = (i + 1) % num_points;
        vec2d pA = my_engine.nodes[i].pos;
        vec2d pB = my_engine.nodes[next_i].pos;

        float dist = std::sqrt((pA.x - pB.x) * (pA.x - pB.x) + (pA.y - pB.y) * (pA.y - pB.y));

        my_engine.springs.push_back({i, next_i, dist, 1000.0f, 15.0f});
    }

    // Inicjalizacja ciśnienia (Pressure Soft Body)
    my_engine.target_area = my_engine.get_volume();
    my_engine.pressure_mult = 1.0f; 

    int grabbedPoint = -1;

    while (!WindowShouldClose()) {
        // ==========================================
        // 1. INPUT MYSZKI
        // ==========================================
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (size_t i = 0; i < my_engine.nodes.size(); ++i) {
                float dx = my_engine.nodes[i].pos.x - mouse.x;
                float dy = my_engine.nodes[i].pos.y - mouse.y;
                if (std::sqrt(dx * dx + dy * dy) < my_engine.nodes[i].radius * 3.0f) {
                    grabbedPoint = (int)i;
                    break;
                }
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            grabbedPoint = -1;
        }

        // ==========================================
        // 2. FIZYKA (Sub-stepping)
        // ==========================================
        
        // Uruchamiamy fizykę wielokrotnie w mniejszych krokach (dt = 0.002) dla super stabilności
        for (int step = 0; step < 8; ++step) {
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y}; 
            }

            my_engine.step();

            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y};
            }
        }

        // ==========================================
        // 3. RENDEROWANIE
        // ==========================================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Renderowanie wypełnienia
        std::vector<Vector2> poly_points;
        vec2d center_pos = {0,0};
        for (const auto& n : my_engine.nodes) {
            center_pos = center_pos + n.pos;
        }
        center_pos = center_pos * (1.0f / my_engine.nodes.size());
        
        poly_points.push_back({center_pos.x, center_pos.y}); // środek na indexie 0
        
        // Zmiana kolejności z CW na CCW (Counter-Clockwise) aby OpenGL nie ukrył trójkątów (Backface Culling)
        for (int i = (int)my_engine.nodes.size() - 1; i >= 0; --i) {
            poly_points.push_back({my_engine.nodes[i].pos.x, my_engine.nodes[i].pos.y});
        }
        poly_points.push_back({my_engine.nodes.back().pos.x, my_engine.nodes.back().pos.y}); // zamknij obwód
        
        DrawTriangleFan(poly_points.data(), poly_points.size(), SKYBLUE);

        // Rysowanie grubych krawędzi balonu (sprężyny)
        for (const auto& spring : my_engine.springs) {
            vec2d pA = my_engine.nodes[spring.idA].pos;
            vec2d pB = my_engine.nodes[spring.idB].pos;
            DrawLineEx({pA.x, pA.y}, {pB.x, pB.y}, 5.0f, BLUE);
        }

        // Rysowanie samych węzłów
        for (const auto& node : my_engine.nodes) {
            DrawCircle((int)node.pos.x, (int)node.pos.y, node.radius, DARKBLUE);
        }

        DrawText("Przeciagnij wezly myszka! (Pressure Soft Body)", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}