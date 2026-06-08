#include "physics/softBodyFactory.h"

#include <cmath>
#include <cstddef>
#include <vector>

#include "physics/softBody.h"
#include "physics/vec2d.h"

//TODO : CUSTOM STIFFNES, MASS

namespace {

void build_rope(engine& eng, const factory::BodyConfig& config, SoftBody& body) {
    size_t num_nodes = static_cast<size_t>(config.num_nodes);

    // Adding Nodes
    // from top to bottom
    for (size_t i = 0; i < num_nodes; ++i) {
        vec2d pos = {
            config.start_pos.x,
            config.start_pos.y + (i * config.spacing)}; 
        eng.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
    }

    // Adding Springs
    // conecting node i + 1 with i - 1
    for (size_t i = 0; i < num_nodes - 1; ++i) {
        size_t current_idx = body.node_start + i;
        size_t next_idx = current_idx + 1;

        eng.springs.push_back({current_idx, next_idx, config.spacing, 1000.0f, 15.0f});
    }
}

void build_balloon(engine& eng, const factory::BodyConfig& config, SoftBody& body) {
    size_t num_points = static_cast<size_t>(config.num_nodes);
    vec2d center = config.start_pos;
    float radius = config.radius;

    for (size_t i = 0; i < num_points; ++i) {
        float angle = i * (2.0f * 3.14159f / num_points);
        vec2d pos = {
            center.x + std::cos(angle) * radius,
            center.y + std::sin(angle) * radius};
        eng.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
    }

    for (size_t i = 0; i < num_points; ++i) {
        size_t current_idx = body.node_start + i;
        size_t next_idx = body.node_start + ((i + 1) % num_points);

        float dist = (eng.nodes[current_idx].pos - eng.nodes[next_idx].pos).length();
        eng.springs.push_back({current_idx, next_idx, dist, 1000.0f, 15.0f});
    }

    body.hull_start = body.node_start;
    body.hull_end = body.node_start + num_points;
    body.target_area = 3.14159f * radius * radius;
    body.pressure_mult = 1.0f;
}

void build_cloth(engine& eng, const factory::BodyConfig& config, SoftBody& body) {
    size_t cols = static_cast<size_t>(config.cols);
    size_t rows = static_cast<size_t>(config.rows);

    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            vec2d pos = {
                config.start_pos.x + x * config.spacing,
                config.start_pos.y + y * config.spacing};
            eng.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
        }
    }

    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            size_t current = body.node_start + y * cols + x;

            // Right
            if (x < cols - 1) {
                eng.springs.push_back({current, current + 1, config.spacing, 800.0f, 10.0f});
            }
            // Down
            if (y < rows - 1) {
                eng.springs.push_back({current, current + cols, config.spacing, 800.0f, 10.0f});
            }
        }
    }
}

void build_jello(engine& eng, const factory::BodyConfig& config, SoftBody& body) {
    size_t cols = static_cast<size_t>(config.cols);
    size_t rows = static_cast<size_t>(config.rows);

    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            vec2d pos = {
                config.start_pos.x + x * config.spacing,
                config.start_pos.y + y * config.spacing};
            eng.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
        }
    }

    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            size_t current = body.node_start + y * cols + x;

            if (x < cols - 1) eng.springs.push_back({current, current + 1, config.spacing, 2000.0f, 15.0f});
            if (y < rows - 1) eng.springs.push_back({current, current + cols, config.spacing, 2000.0f, 15.0f});

            // Cross Springs
            if (x < cols - 1 && y < rows - 1) {
                float diag_dist = config.spacing * 1.41421356f;  // spacing * sqrt(2)
                eng.springs.push_back({current, current + cols + 1, diag_dist, 1500.0f, 15.0f});
                eng.springs.push_back({current + 1, current + cols, diag_dist, 1500.0f, 15.0f});
            }
        }
    }
}

void build_ball(engine& eng, const factory::BodyConfig& config, SoftBody& body) {
    size_t num_points = static_cast<size_t>(config.num_nodes);
    vec2d center = config.start_pos;
    float radius = config.radius;

    //Perimeter
    for (size_t i = 0; i < num_points; ++i) {
        float angle = i * (2.0f * 3.14159f / num_points);
        vec2d pos = {
            center.x + std::cos(angle) * radius,
            center.y + std::sin(angle) * radius};
        eng.nodes.push_back({pos, pos, {0.0f, 0.0f}, 1.0f, 6.0f});
    }

    size_t center_idx = body.node_start + num_points;
    eng.nodes.push_back({center, center, {0.0f, 0.0f}, 1.0f, 6.0f});

    // Adding Springs
    for (size_t i = 0; i < num_points; ++i) {
        size_t current_idx = body.node_start + i;
        size_t next_idx = body.node_start + ((i + 1) % num_points);

        float dist = (eng.nodes[current_idx].pos - eng.nodes[next_idx].pos).length();
        eng.springs.push_back({current_idx, next_idx, dist, 1000.0f, 15.0f});
        float dist_center = (eng.nodes[current_idx].pos - eng.nodes[center_idx].pos).length();
        eng.springs.push_back({current_idx, center_idx, dist_center, 800.0f, 15.0f});
    }

    body.hull_start = body.node_start;
    body.hull_end = body.node_start + num_points;  
    body.target_area = 0.0f;
    body.pressure_mult = 0.0f;
}

}  // namespace

namespace factory {

SoftBody create_body(BodyType type, engine& eng, const BodyConfig& config) {
    SoftBody body;
    body.type = type;

    body.node_start = eng.nodes.size();
    body.spring_start = eng.springs.size();

    switch (type) {
        case BodyType::Rope:
            build_rope(eng, config, body);
            break;
        case BodyType::Balloon:
            build_balloon(eng, config, body);
            break;
        case BodyType::Cloth:
            build_cloth(eng, config, body);
            break;
        case BodyType::Jello:
            build_jello(eng, config, body);
            break;
        case BodyType::Ball:
            build_ball(eng, config, body);
            break;
        default:
            break;
    }

    body.node_end = eng.nodes.size();
    body.spring_end = eng.springs.size();
    return body;
}

}  // namespace factory