#ifndef VEC_2D_H
#define VEC_2D_H
#include <cmath>

struct vec2d {
    float x;
    float y;

    //operators
    vec2d operator-(const vec2d& other) const noexcept { return {x - other.x, y - other.y}; }
    vec2d operator*(float a) const noexcept { return {x * a, y * a}; }
    vec2d& operator+=(const vec2d& other) noexcept {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    vec2d& operator-=(const vec2d& other) noexcept {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    float length() const noexcept {return std::sqrt(x * x + y * y);};
    vec2d normalize() const {
        float len = length();
        if (len <= 0.0001f) return {0.0f, 0.0f};
        return {x / len, y / len};
    };
};
#endif