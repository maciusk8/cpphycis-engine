#include <raylib.h>

#include <cmath>

#include "engine.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Test Silnika Fizycznego");
    SetTargetFPS(60);

    engine my_engine;

    // --- GENEROWANIE PIĘCIOKĄTA ---
    const int num_points = 5;
    const float radius = 100.0f;
    const Vector2 center = {400.0f, 300.0f};

    for (int i = 0; i < num_points; ++i) {
        float angle = i * (2.0f * 3.14159f / num_points) - (3.14159f / 2.0f);

        my_engine.positions.push_back({center.x + std::cos(angle) * radius,
                                       center.y + std::sin(angle) * radius});
        my_engine.velocities.push_back({0.0f, 0.0f});
        my_engine.forces.push_back({0.0f, 0.0f});
    }

    // --- ŁĄCZENIE SPRĘŻYN (Graf pełny) ---
    for (int i = 0; i < num_points; ++i) {
        for (int j = i + 1; j < num_points; ++j) {
            vec2d pA = my_engine.positions[i];
            vec2d pB = my_engine.positions[j];

            float dist = std::sqrt((pA.x - pB.x) * (pA.x - pB.x) + (pA.y - pB.y) * (pA.y - pB.y));

            my_engine.springs.push_back({i, j, dist, 5.0f});
        }
    }

    int grabbedPoint = -1;

    while (!WindowShouldClose()) {
        // ==========================================
        // 1. INPUT MYSZKI
        // ==========================================
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (size_t i = 0; i < my_engine.positions.size(); ++i) {
                float dx = my_engine.positions[i].x - mouse.x;
                float dy = my_engine.positions[i].y - mouse.y;
                if (std::sqrt(dx * dx + dy * dy) < 20.0f) {
                    grabbedPoint = (int)i;
                    break;
                }
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            grabbedPoint = -1;
        }

        // ==========================================
        // 2. FIZYKA
        // ==========================================
        my_engine.step();

        if (grabbedPoint != -1) {
            my_engine.positions[grabbedPoint] = {mouse.x, mouse.y};
            my_engine.velocities[grabbedPoint] = {0.0f, 0.0f};
        }

        // ==========================================
        // 3. RENDEROWANIE
        // ==========================================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const auto& spring : my_engine.springs) {
            vec2d pA = my_engine.positions[spring.idA];
            vec2d pB = my_engine.positions[spring.idB];
            DrawLineEx({pA.x, pA.y}, {pB.x, pB.y}, 5.0f, LIGHTGRAY);
        }

        for (const auto& pos : my_engine.positions) {
            DrawCircle((int)pos.x, (int)pos.y, 15.0f, DARKBLUE);
        }

        DrawText("Przeciagnij wezly myszka!", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}