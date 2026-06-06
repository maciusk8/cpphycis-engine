#include "physics/engine.h"
#include <cmath>

float engine::get_volume() const noexcept {
    float area = 0.0f;
    for (size_t i = 0; i < nodes.size(); ++i) {
        size_t next_i = (i + 1) % nodes.size();
        area += (nodes[i].pos.x * nodes[next_i].pos.y - nodes[next_i].pos.x * nodes[i].pos.y);
    }
    return std::abs(area * 0.5f);
}

void engine::apply_forces() noexcept {
    for (const auto& spring : springs) {
        Node& nA = nodes[spring.idA];
        Node& nB = nodes[spring.idB];
        
        vec2d vec = nA.pos - nB.pos;
        float dist = vec.length();
        if (dist == 0.0f) continue;
        
        vec2d dir = vec.normalize();
        
        // F = kx (Prawo Hooke'a)
        float x = spring.resting_length - dist;
        vec2d force_spring = dir * (x * spring.stiffness);
        
        // Tłumienie (damping)
        vec2d vA = (nA.pos - nA.prev_pos) * (1.0f / dt);
        vec2d vB = (nB.pos - nB.prev_pos) * (1.0f / dt);
        vec2d relative_vel = vA - vB;
        
        float vel_along_spring = relative_vel.x * dir.x + relative_vel.y * dir.y;
        vec2d force_damping = dir * (-spring.damping * vel_along_spring);
        
        vec2d total_force = force_spring + force_damping;
        
        nA.force += total_force;
        nB.force -= total_force;
    }

    // Ciśnienie gazu
    if (pressure_mult > 0.0f && nodes.size() >= 3) {
        float current_area = get_volume();
        float pressure_force = (target_area - current_area) * pressure_mult;

        for (size_t i = 0; i < nodes.size(); ++i) {
            size_t next_i = (i + 1) % nodes.size();
            vec2d pA = nodes[i].pos;
            vec2d pB = nodes[next_i].pos;

            vec2d edge = pB - pA;
            float edge_len = edge.length();
            if (edge_len == 0.0f) continue;

            vec2d normal = {edge.y / edge_len, -edge.x / edge_len};

            vec2d force = normal * (pressure_force * edge_len * 0.5f);
            nodes[i].force += force;
            nodes[next_i].force += force;
        }
    }
}

// Całkowanie Verlet dla większej stabilności
void engine::integrate() noexcept {
    for (auto& node : nodes) {
        // Obliczenie przyspieszenia (a = F / m + grawitacja)
        vec2d acc = (node.force * (1.0f / node.mass)) + gravity;
        
        // Zapisanie obecnej pozycji
        vec2d current_pos = node.pos;
        
        // Obliczenie nowej pozycji (Verlet)
        // pos_new = pos + (pos - prev_pos) + a * dt^2
        node.pos = node.pos + (node.pos - node.prev_pos) + acc * (dt * dt);
        
        // Zaktualizowanie poprzedniej pozycji
        node.prev_pos = current_pos;
        
        // Kolizja z podłogą (Y = 600)
        if (node.pos.y > 800.0f - node.radius) {
            node.pos.y = 800.0f - node.radius;
            // Proste odbicie i tarcie z podłogą
            float vx = node.pos.x - node.prev_pos.x;
            node.prev_pos.x = node.pos.x - vx * 0.9f; // tarcie
            node.prev_pos.y = node.pos.y; // zerowanie prędkości pionowej (lub odbicie)
        }
        
        // Kolizja ze ścianami
        if (node.pos.x < node.radius) {
            node.pos.x = node.radius;
            node.prev_pos.x = node.pos.x;
        } else if (node.pos.x > 1200.0f - node.radius) {
            node.pos.x = 1200.0f - node.radius;
            node.prev_pos.x = node.pos.x;
        }

        // Wyzerowanie sił na następną klatkę
        node.force = {0.0f, 0.0f};
    }
}

void engine::step() noexcept {
    apply_forces();
    integrate();
}

void engine::create_blob(vec2d center, float radius, int num_points) noexcept {
    // Generowanie punktów
    for (int i = 0; i < num_points; ++i) {
        float angle = i * (2.0f * 3.14159f / num_points);
        vec2d pos = {center.x + std::cos(angle) * radius,
                     center.y + std::sin(angle) * radius};
        nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
    }

    // Łączenie sprężyn na obwodzie
    for (int i = 0; i < num_points; ++i) {
        int next_i = (i + 1) % num_points;
        vec2d pA = nodes[i].pos;
        vec2d pB = nodes[next_i].pos;
        float dist = (pA - pB).length();
        springs.push_back({i, next_i, dist, 1000.0f, 15.0f});
    }

    target_area = get_volume();
    pressure_mult = 1.0f;
}