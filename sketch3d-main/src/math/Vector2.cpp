#include "math/Vector2.h"

namespace Sketch3D {

const Vector2 Vector2::ZERO (0.0f, 0.0f);
const Vector2 Vector2::RIGHT(1.0f, 0.0f);
const Vector2 Vector2::UP   (0.0f, 1.0f);
const Vector2 Vector2::ONE  (1.0f, 1.0f);

Vector2::Vector2() : x(0.0f), y(0.0f) {
}

Vector2::Vector2(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector2::Vector2(const Vector2& src) {
    x = src.x;
    y = src.y;
}

float Vector2::Length() const {
    return sqrtf(x*x + y*y);
}

float Vector2::SquaredLength() const {
    return x*x + y*y;
}

Vector2 Vector2::Normalized() const {
    float f = Length();
    return Vector2(x / f, y / f);
}

void Vector2::Normalize() {
    float f = Length();
    x /= f;
    y /= f;
}

float Vector2::Dot(const Vector2& v) const {
    return x * v.x + y * v.y;
}

Vector2 Vector2::Perpendicular() const {
    return Vector2(-y, x);
}

}