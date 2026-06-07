#include <raylib.h>
#include <cmath>
#include "physics/engine.h"
#include "render/renderer.h"
#include "ui/ui.h"

int main() {
    InitWindow(1200, 800, "Test Silnika Fizycznego");
    SetTargetFPS(240);

    // UI
    UI ui;
    ui.init();

    engine my_engine;
    my_engine.create_blob({600.0f, 400.0f}, 150.0f, 40);

    Renderer renderer;
    int grabbedPoint = -1;
    //akumulator czasu
    double accumulator = 0.0;

    while (!WindowShouldClose()) {
        double frame_time = GetFrameTime();
        if (frame_time > 0.1) frame_time = 0.1;

        accumulator += frame_time * ui.get_time_scale();
        // --- 1. INPUT ---
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !ui.wants_mouse_capture()) {
            for (size_t i = 0; i < my_engine.nodes.size(); ++i) {
                float dx = my_engine.nodes[i].pos.x - mouse.x;
                float dy = my_engine.nodes[i].pos.y - mouse.y;
                if (std::sqrt(dx * dx + dy * dy) < my_engine.nodes[i].radius * 3.0f) {
                    grabbedPoint = static_cast<int>(i);
                    break;
                }
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) grabbedPoint = -1;

        // --- 2. FIZYKA ---
        while (accumulator >= my_engine.dt) {
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y}; 
            }
            my_engine.step();
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y};
            }
            accumulator -= my_engine.dt;
        }

        // --- 3. RENDER ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderer.draw_world(my_engine, ui.get_curve_segments());
        ui.draw(my_engine);

        EndDrawing();
    }

    ui.shutdown();
    CloseWindow();
    return 0;
}