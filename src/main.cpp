#include <raylib.h>
#include <cmath>
#include <vector>
#include "physics/engine.h"
#include "physics/softBodyFactory.h"
#include "render/renderer.h"

int main() {
    InitWindow(1200, 800, "Test Silnika Fizycznego");
    SetTargetFPS(240);

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

    while (!WindowShouldClose()) {
        // --- 1. INPUT ---
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
        for (int step = 0; step < 8; ++step) {
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y}; 
            }
            my_engine.step(bodies);
            if (grabbedPoint != -1) {
                my_engine.nodes[grabbedPoint].pos = {mouse.x, mouse.y};
                my_engine.nodes[grabbedPoint].prev_pos = {mouse.x, mouse.y};
            }
        }

        // --- 3. RENDER ---
        renderer.draw_world(my_engine, bodies);
    }

    CloseWindow();
    return 0;
}