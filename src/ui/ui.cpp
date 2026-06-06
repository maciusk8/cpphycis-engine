#include "ui/ui.h"
#include "imgui.h"
#include "rlImGui.h"

void UI::init() const noexcept {
    rlImGuiSetup(true);
}

void UI::shutdown() const noexcept {
    rlImGuiShutdown();
}

bool UI::wants_mouse_capture() const noexcept {
    return ImGui::GetIO().WantCaptureMouse;
}

void UI::draw(engine& phys_engine) noexcept {
    rlImGuiBegin();
    ImGui::Begin("sim control panel");

    ImGui::Text("global properties");
    ImGui::SliderFloat("gravity y", &phys_engine.gravity.y, -2000.0f, 2000.0f);
    ImGui::SliderFloat("gravity x", &phys_engine.gravity.x, -2000.0f, 2000.0f);
    
    ImGui::Separator();
    ImGui::Text("blob properties");
    ImGui::SliderFloat("pressure", &phys_engine.pressure_mult, 0.0f, 30.0f);
    
    ImGui::Separator();
    ImGui::Text("spawner settings");
    ImGui::SliderFloat("radius", &next_radius, 30.0f, 400.0f);
    ImGui::SliderInt("points count", &next_num_points, 3, 100);
    
    if (ImGui::Button("blob spawn")) {
        phys_engine.nodes.clear();
        phys_engine.springs.clear();
        phys_engine.create_blob({600.0f, 400.0f}, next_radius, next_num_points);
    }

    ImGui::End();
    rlImGuiEnd();
}