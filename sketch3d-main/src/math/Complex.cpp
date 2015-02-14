#include "math/Complex.h"

namespace Sketch3D {

Complex::Complex() : a(0.0f), b(0.0f) {
}

Complex::Complex(float a, float b) {
    this->a = a;
    this->b = b;
}

Complex::Complex(const Complex& c) {
    a = c.a;
    b = c.b;
}

Complex Complex::Conjugate() const {
    return Complex(a, -b);
}

}