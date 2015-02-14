#ifndef SKETCH_3D_COMPLEX_H
#define SKETCH_3D_COMPLEX_H

#include "system/Common.h"

namespace Sketch3D {

/**
 * @class Complex
 * Defines a complex number a + i*b where i is the imaginary number
 */
class SKETCH_3D_API Complex {
    public:
        float                   a;  /**< Real part */
        float                   b;  /**< Imaginary part */

        /**
         * Default constructor. Initializes everything to 0.
         */
                                Complex();

        /**
         * Constructor
         * @param real The real part
         * @param imag The imaginary part
         */
                                Complex(float real, float imag);

        /**
         * Copy constructor
         * @param src The src object to copy
         */
                                Complex(const Complex& src);

        /**
         * Returns the conjugate of the number
         */
        Complex                 Conjugate() const;

        // UNARY OPERATORS
        INLINE Complex          operator-() const;

        // BINARY OPERATORS
        INLINE Complex          operator+(const Complex& c) const;
        INLINE Complex          operator-(const Complex& c) const;
        INLINE Complex          operator*(const Complex& c) const;
        INLINE Complex          operator*(float f) const;

        INLINE friend Complex   operator*(float f, const Complex& c);

        INLINE void             operator+=(const Complex& c);
        INLINE void             operator-=(const Complex& c);
        INLINE void             operator*=(const Complex& c);
        INLINE void             operator*=(float f);

        INLINE bool             operator==(const Complex& c) const;
        INLINE bool             operator!=(const Complex& c) const;

        INLINE Complex&         operator=(const Complex& c);
};

INLINE Complex Complex::operator-() const {
    return Complex(-a, -b);
}

INLINE Complex Complex::operator+(const Complex& c) const {
    return Complex(a + c.a, b + c.b);
}

INLINE Complex Complex::operator-(const Complex& c) const {
    return Complex(a - c.a, b - c.b);
}

INLINE Complex Complex::operator*(const Complex& c) const {
    float k1 = a * (c.a + c.b);
    float k2 = c.b * (a + b);
    float k3 = c.a * (b - a);
    return Complex(k1 - k2, k1 + k3);
}

INLINE Complex Complex::operator*(float f) const {
    return Complex(a * f, b * f);
}

INLINE Complex operator*(float f, const Complex& c) {
    return Complex(c.a * f, c.b * f);
}

INLINE void Complex::operator+=(const Complex& c) {
    a += c.a;
    b += c.b;
}

INLINE void Complex::operator-=(const Complex& c) {
    a -= c.a;
    b -= c.b;
}

INLINE void Complex::operator*=(const Complex& c) {
    float k1 = a * (c.a + c.b);
    float k2 = c.b * (a + b);
    float k3 = c.a * (b - a);
    a = k1 - k2;
    b = k1 + k3;
}

INLINE void Complex::operator*=(float f) {
    a *= f;
    b *= f;
}

INLINE bool Complex::operator==(const Complex& c) const {
    return (fabs(a - c.a) < 0.00001f && fabs(b - c.b) < 0.00001f);
}

INLINE bool Complex::operator!=(const Complex& c) const {
    return (fabs(a - c.a) >= 0.00001f || fabs(b - c.b) >= 0.00001f);
}

INLINE Complex& Complex::operator=(const Complex& c) {
    if (this != &c) {
        a = c.a;
        b = c.b;
    }

    return *this;
}

}

#endif