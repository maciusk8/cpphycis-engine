#ifndef SOFT_BODY_H
#define SOFT_BODY_H

#include <cstddef>


enum class BodyType {
    Rope,
    Cloth,  // or flag
    Jello,
    Balloon,
    Ball,
    // Maybe Custom in future
};

/* SoftBody acts as a "view" (span/slice), mapping a logical object 
 * to contiguous memory segments within the global engine vectors
 * (engine::nodes, engine::springs).
 *
 *  Range convention: [start, end) - 'end' is the first index outside the slice (exclusive).
 */

struct SoftBody {
    BodyType type;

    //One Soft body is represested as "slice" "googles" 
    // Indexes in global vectors
    size_t node_start;
    size_t node_end;

    //Springs
    size_t spring_start;
    size_t spring_end;

    //requiured for pressure - used with Baloon and Ball
    size_t hull_start;
    size_t hull_end;
    float target_area = 0.0f;
    float pressure_mult = 0.0f;
};
#endif  // SoftBody