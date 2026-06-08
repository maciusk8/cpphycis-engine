#ifndef SOFT_BODY_FACTORY_H
#define SOFT_BODY_FACTORY_H

#include "physics/engine.h"
#include "physics/softBody.h"
#include "physics/vec2d.h"

namespace factory {
struct BodyConfig {
    vec2d start_pos = {0.0f, 0.0f};
    float spacing = 20.0f; 
    int num_nodes = 10;    // In one line
    int cols = 5;          // For Cloth, Jello
    int rows = 5;          // For Cloth, Jello
    float radius = 50.0f;  // for Ball, Balloon
};

SoftBody create_body(BodyType type, engine& eng, const BodyConfig& config);
}  // namespace factory
#endif  // SOFT_BODY_FACTORY_H