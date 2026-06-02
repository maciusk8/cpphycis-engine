#ifndef SPRING_H
#define SPRING_H
// spring connects points with id=A with id = B;
struct Spring {
    int idA;
    int idB;
    float resting_length; 
    float stiffness;
};

#endif