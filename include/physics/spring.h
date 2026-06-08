#ifndef SPRING_H
#define SPRING_H
// spring connects points with id=A with id = B;
#include <cstddef>
struct Spring {
    size_t idA;
    size_t idB;
    float resting_length; 
    float stiffness;
    float damping;
};

#endif