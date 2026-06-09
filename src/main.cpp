#include <raylib.h>
#include <cmath>
#include <vector>
#include "physics/engine.h"
#include "physics/softBodyFactory.h"
#include "render/renderer.h"
#include "ui/ui.h"

int main() {
    InitWindow(1200, 800, "Test Silnika Fizycznego");
    SetTargetFPS(240);

    // UI
    UI ui;
    ui.init();

    engine my_engine;
    std::vector<SoftBody> bodies;
    
    factory::BodyConfig rope_config;
    rope_config.start_pos = {100.0f, 100.0f};
    rope_config.num_nodes = 15;
    rope_config.spacing = 25.0f;
    bodies.push_back(factory::create_body(BodyType::Rope, my_engine, rope_config));

    factory::BodyConfig jello_config;
    jello_config.start_pos = {400.0f, 200.0f};
    jello_config.cols = 6;
    jello_config.rows = 6;
    jello_config.spacing = 20.0f;
    bodies.push_back(factory::create_body(BodyType::Jello, my_engine, jello_config));

    factory::BodyConfig balloon_config;
    balloon_config.start_pos = {800.0f, 200.0f};
    balloon_config.num_nodes = 20;
    balloon_config.radius = 70.0f;
    bodies.push_back(factory::create_body(BodyType::Balloon, my_engine, balloon_config));

    factory::BodyConfig cloth_config;
    cloth_config.start_pos = {1000.0f, 100.0f};
    cloth_config.cols = 8;
    cloth_config.rows = 8;
    cloth_config.spacing = 15.0f;
    bodies.push_back(factory::create_body(BodyType::Cloth, my_engine, cloth_config));

    factory::BodyConfig ball_config;
    ball_config.start_pos = {600.0f, 400.0f};
    ball_config.num_nodes = 16;
    ball_config.radius = 50.0f;
    bodies.push_back(factory::create_body(BodyType::Ball, my_engine, ball_config));

    Renderer renderer;
    int grabbedPoint = -1;
    //akumulator czasu
    double accumulator = 0.0;

    Camera2D camera = { 0 };
    camera.target = { 600.0f, 800.0f };
    camera.offset = { 600.0f, 800.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float manual_target_zoom = 1.0f;

    while (!WindowShouldClose()) {
        double frame_time = GetFrameTime();
        if (frame_time > 0.1) frame_time = 0.1;

        accumulator += frame_time * ui.get_time_scale();
        
        // --- 0. OBSŁUGA KAMERY ---
        if (ui.is_auto_camera_enabled()) {
            float min_y = 800.0f;
            float min_x = 600.0f, max_x = 600.0f;
            if (!my_engine.nodes.empty()) {
                min_x = my_engine.nodes[0].pos.x;
                max_x = my_engine.nodes[0].pos.x;
            }
            for (const auto& node : my_engine.nodes) {
                if (node.pos.y < min_y) min_y = node.pos.y;
                if (node.pos.x < min_x) min_x = node.pos.x;
                if (node.pos.x > max_x) max_x = node.pos.x;
            }
            
            float target_zoom_y = 1.0f;
            if (min_y < 100.0f) {
                target_zoom_y = 800.0f / (800.0f - min_y + 100.0f);
            }
            float width_needed = max_x - min_x + 200.0f;
            float target_zoom_x = 1200.0f / width_needed;
            
            float target_zoom = std::fmin(target_zoom_x, target_zoom_y);
            if (target_zoom > 1.0f) target_zoom = 1.0f;
            
            camera.zoom += (target_zoom - camera.zoom) * 5.0f * GetFrameTime();
            
            float center_x = (min_x + max_x) * 0.5f;
            camera.target.x += (center_x - camera.target.x) * 5.0f * GetFrameTime();
            
            camera.offset = {600.0f, 800.0f};
            camera.target.y = 800.0f;
            
            manual_target_zoom = camera.zoom; 
            
        } else {
            float wheel = GetMouseWheelMove();
            
            if (wheel != 0.0f && !ui.wants_mouse_capture()) {
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;

                float scaleFactor = 1.0f + (0.15f * std::abs(wheel));
                if (wheel < 0) scaleFactor = 1.0f / scaleFactor;
                
                manual_target_zoom *= scaleFactor;
                if (manual_target_zoom < 0.1f) manual_target_zoom = 0.1f;
                if (manual_target_zoom > 10.0f) manual_target_zoom = 10.0f;
            }

            camera.zoom += (manual_target_zoom - camera.zoom) * 15.0f * GetFrameTime();

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !ui.wants_mouse_capture()) {
                Vector2 delta = GetMouseDelta();
                camera.target.x -= delta.x / camera.zoom;
                camera.target.y -= delta.y / camera.zoom;
            }
        }

        // --- 1. INPUT ---
        Vector2 mouse = GetMousePosition();
        Vector2 world_mouse = GetScreenToWorld2D(mouse, camera);

        bool clicked_on_node = false;
        static Vector2 last_paint_pos = {-10000.0f, -10000.0f};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !ui.wants_mouse_capture()) {
            for (size_t i = 0; i < my_engine.nodes.size(); ++i) {
                float dx = my_engine.nodes[i].pos.x - world_mouse.x;
                float dy = my_engine.nodes[i].pos.y - world_mouse.y;
                if (std::sqrt(dx * dx + dy * dy) < my_engine.nodes[i].radius * 3.0f) {
                    grabbedPoint = static_cast<int>(i);
                    clicked_on_node = true;
                    break;
                }
            }
            
            if (!clicked_on_node && ui.is_spawn_on_click_enabled()) {
                ui.spawn_current_selection(my_engine, bodies, world_mouse.x, world_mouse.y);
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && ui.is_brush_mode_enabled() && !ui.wants_mouse_capture()) {
            float dx = world_mouse.x - last_paint_pos.x;
            float dy = world_mouse.y - last_paint_pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > my_engine.node_radius * 1.5f) {
                const float* col = ui.get_spawn_color();
                SimColor brush_color = {
                    static_cast<unsigned char>(col[0] * 255.0f),
                    static_cast<unsigned char>(col[1] * 255.0f),
                    static_cast<unsigned char>(col[2] * 255.0f),
                    255
                };

                my_engine.nodes.push_back({
                    {world_mouse.x, world_mouse.y}, 
                    {world_mouse.x, world_mouse.y}, 
                    {0.0f, 0.0f}, 
                    0.0f,
                    my_engine.node_radius,
                    brush_color
                });
                last_paint_pos = world_mouse;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            grabbedPoint = -1;
            last_paint_pos = {-10000.0f, -10000.0f};
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) grabbedPoint = -1;

        // --- 2. FIZYKA ---
        while (accumulator >= my_engine.dt) {
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {world_mouse.x, world_mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {world_mouse.x, world_mouse.y}; 
            }
            my_engine.step(bodies);
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {world_mouse.x, world_mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {world_mouse.x, world_mouse.y};
            }
            accumulator -= my_engine.dt;
        }

        // --- 3. RENDER ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
        renderer.draw_world(my_engine, bodies, ui.get_curve_segments());
        EndMode2D();
        
        DrawText("Przeciagnij wezly myszka! (Pressure Soft Body)", 10, 10, 20, DARKGRAY);
        DrawFPS(10, 40);
        DrawText(TextFormat("Bodies: %d", static_cast<int>(bodies.size())), 10, 70, 20, DARKGRAY);

        ui.draw(my_engine, bodies);

        EndDrawing();
    }

    ui.shutdown();
    CloseWindow();
    return 0;
}