#include "ui/ui.h"
#include "imgui.h"
#include "rlImGui.h"
#include "physics/softBodyFactory.h"
#include <cstdlib>

void UI::init() const noexcept {
    rlImGuiSetup(true);
}

void UI::shutdown() const noexcept {
    rlImGuiShutdown();
}

bool UI::wants_mouse_capture() const noexcept {
    return ImGui::GetIO().WantCaptureMouse;
}

void UI::draw(engine& phys_engine, std::vector<SoftBody>& bodies) noexcept {
    rlImGuiBegin();
    ImGui::Begin("sim control panel");

    ImGui::Text("engine performance");
    if (ImGui::SliderInt("fps target", &target_fps, 30, 500)) {
        SetTargetFPS(target_fps);
    }
    ImGui::SliderFloat("time step (dt)", &phys_engine.dt, 0.0001f, 0.01f, "%.4f");
    ImGui::SliderFloat("time scale", &time_scale, 0.01f, 10.0f, "%.2fx");
    ImGui::Separator();
    ImGui::Text("render settings");
    ImGui::SliderInt("curve resolution", &curve_segments, 1, 30);
    ImGui::Separator();

    ImGui::Text("global properties");
    ImGui::SliderFloat("gravity y", &phys_engine.gravity.y, -2000.0f, 2000.0f);
    ImGui::SliderFloat("gravity x", &phys_engine.gravity.x, -2000.0f, 2000.0f);
    ImGui::SliderFloat("node radius", &phys_engine.node_radius, 1.0f, 20.0f);
    ImGui::SliderFloat("collision stiffness", &phys_engine.collision_stiffness, 0.1f, 1.5f);
    
    ImGui::Separator();
    ImGui::Text("spawner settings");
    const char* body_names[] = { "Rope", "Cloth", "Jello", "Balloon", "Ball" };
    ImGui::Combo("Body Type", &selected_body_type, body_names, IM_ARRAYSIZE(body_names));
    ImGui::InputInt("spawn count", &spawn_count);
    if (spawn_count < 1) spawn_count = 1;
    
    if (ImGui::Button("Spawn selected body")) {
        for (int i = 0; i < spawn_count; ++i) {
            factory::BodyConfig config;
            float offset_x = static_cast<float>(rand() % 600 - 300);
            float offset_y = static_cast<float>(rand() % 400 - 200);
            config.start_pos = {600.0f + offset_x, 200.0f + offset_y};

            BodyType type = static_cast<BodyType>(selected_body_type);
            switch (type) {
                case BodyType::Rope:
                    config.num_nodes = 15;
                    config.spacing = 25.0f;
                    break;
                case BodyType::Cloth:
                    config.cols = 8;
                    config.rows = 8;
                    config.spacing = 15.0f;
                    break;
                case BodyType::Jello:
                    config.cols = 6;
                    config.rows = 6;
                    config.spacing = 20.0f;
                    break;
                case BodyType::Balloon:
                    config.num_nodes = 20;
                    config.radius = 70.0f;
                    break;
                case BodyType::Ball:
                    config.num_nodes = 16;
                    config.radius = 50.0f;
                    break;
            }
            bodies.push_back(factory::create_body(type, phys_engine, config));
        }
    }

    if (ImGui::Button("Clear All Bodies")) {
        phys_engine.nodes.clear();
        phys_engine.springs.clear();
        bodies.clear();
    }

    ImGui::End();
    rlImGuiEnd();
}