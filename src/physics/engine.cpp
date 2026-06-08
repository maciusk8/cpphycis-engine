#include "physics/engine.h"

#include <cmath>
#include <cstddef>

// Computes field using nodes on 2D space
// Shoelace Formula
// required: Nodes according to the order of the perimeter
float engine::get_area(size_t start, size_t end) const noexcept {
    float area = 0.0f;
    size_t count = end - start;
    if (count < 3) return 0.0f;
    for (size_t i = start; i < end; ++i) {
        size_t next_i = start + ((i - start + 1) % count);
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
        vec2d vA = (nA.pos - nA.prev_pos) * (1.0f / dt);  // velocity vector
        vec2d vB = (nB.pos - nB.prev_pos) * (1.0f / dt);
        vec2d relative_vel = vA - vB;
        float vel_along_spring = relative_vel.x * dir.x + relative_vel.y * dir.y;
        vec2d force_damping = dir * (-spring.damping * vel_along_spring);
        vec2d total_force = force_spring + force_damping;

        nA.force += total_force;
        nB.force -= total_force;
    }
}

void engine::apply_pressure(const std::vector<SoftBody>& bodies) noexcept {
    for (const auto& body : bodies) {
        size_t count = body.hull_end - body.hull_start;
        if (body.pressure_mult > 0.0f && count >= 3) {
            float current_area = get_area(body.hull_start, body.hull_end);
            float pressure_force = (body.target_area - current_area) * body.pressure_mult;
            for (size_t i = body.hull_start; i < body.hull_end; ++i) {
                size_t next_i = body.hull_start + ((i - body.hull_start + 1) % count);
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
}

// Całkowanie Verleta
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
            node.prev_pos.x = node.pos.x - vx * 0.9f;  // tarcie
            node.prev_pos.y = node.pos.y;              // zerowanie prędkości pionowej (lub odbicie)
        }

        // Kolizja ze ścianami USUNIĘTA - brak ograniczeń poziomo

        // Wyzerowanie sił na następną klatkę
        node.force = {0.0f, 0.0f};
    }
}

void engine::apply_collisions() noexcept {
    float diameter = node_radius * 2.0f;
    float diameter_sq = diameter * diameter;
    
    if (grid.cell_size < diameter) {
        grid = SpatialGrid(1200.0f, 800.0f, diameter);
    }
    
    grid.build(nodes);
    int num_nodes = static_cast<int>(nodes.size());

    for (int i = 0; i < num_nodes; ++i) {
        vec2d pA = nodes[i].pos;

        int cx = static_cast<int>(std::floor(pA.x / grid.cell_size));
        int cy = static_cast<int>(std::floor(pA.y / grid.cell_size));
        
        // Optymalizacja: Sprawdzamy tylko 5 komórek "do przodu", by nie dublować par
        const int offsets[5][2] = {
            {0, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}
        };

        for (int k = 0; k < 5; ++k) {
            int nx = cx + offsets[k][0];
            int ny = cy + offsets[k][1];
            unsigned int hash = grid.get_hash(nx, ny);
            
            int start = grid.starts[hash];
            int count = grid.counts[hash];
            
            for (int idx = 0; idx < count; ++idx) {
                int j = grid.entries[start + idx];
                
                // Jeśli jesteśmy w tej samej komórce, sprawdzamy tylko i < j by uniknąć podwójnej pracy i kolizji z samym sobą
                if (k == 0 && i >= j) continue;
                
                // Zabezpieczenie przed podwójnym sprawdzeniem dla kolizji hashy z różnych komórek
                if (k > 0 && i == j) continue;

                vec2d diff = pA - nodes[j].pos;
                float dist_sq = diff.x * diff.x + diff.y * diff.y;

                if (dist_sq < diameter_sq && dist_sq > 0.00001f) {
                    float dist = std::sqrt(dist_sq);
                    float overlap = diameter - dist;
                    vec2d normal = diff * (1.0f / dist);
                    vec2d correction = normal * (overlap * 0.5f * collision_stiffness);
                    nodes[i].pos = nodes[i].pos + correction;
                    nodes[j].pos = nodes[j].pos - correction;
                }
            }
        }
    }
}

void engine::step(const std::vector<SoftBody>& bodies) noexcept {
    apply_forces();
    apply_pressure(bodies);
    integrate();
    apply_collisions();
}
