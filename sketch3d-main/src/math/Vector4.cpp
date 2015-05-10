#include "math/Vector4.h"

#include "math/Constants.h"
#include "math/Matrix4x4.h"
#include "system/Platform.h"

#include <iostream>
#include <math.h>

namespace Sketch3D {

const Vector4 Vector4::ZERO (0.0f, 0.0f, 0.0f);
const Vector4 Vector4::ONE  (1.0f, 1.0f, 1.0f);
const Vector4 Vector4::RIGHT(1.0f, 0.0f, 0.0f);
const Vector4 Vector4::UP   (0.0f, 1.0f, 0.0f);
const Vector4 Vector4::LOOK (0.0f, 0.0f, 1.0f);

Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
}

Vector4::Vector4(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    w = 1.0f;
}

Vector4::Vector4(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vector4::Vector4(const Vector4& src)
{
    x = src.x;
    y = src.y;
    z = src.z;
    w = 1.0f;
}

float Vector4::Length() const
{
    float f;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        f = sqrtf(x*x + y*y + z*z);
    } else {
#if	PLATFORM == PLATFORM_WIN32
		Vector4 v(x, y, z);
		v.w = 0.0;
		
		float* pf = &f;
		Vector4* pv = &v;

		__asm {
			mov		ecx, pf
			mov		esi, pv
			movups	xmm0, [esi]
			mulps	xmm0, xmm0
			movaps	xmm1, xmm0
            movhlps xmm1, xmm1
			addps	xmm0, xmm1
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 39h
			addps	xmm0, xmm1
			sqrtss	xmm0, xmm0
			movss	[ecx], xmm0
		}

#elif PLATFORM == PLATFORM_LINUX
		f = sqrtf(x*x + y*y + z*z);
#endif
    }

    return f;
}

float Vector4::SquaredLength() const
{
    return x*x + y*y + z*z;
}

Vector4 Vector4::Normalized() const
{
    Vector4 result;
    float length;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        length = Length();
        if (length != 0.0f) {
            result.x = x / length;
            result.y = y / length;
            result.z = z / length;
        }
    } else {
#if	PLATFORM == PLATFORM_WIN32
		Vector4 v(x, y, z);
		v.w = 0.0;
		Vector4* pr = &result;
		Vector4* pv = &v;

		__asm {
			mov		ecx, pr
			mov		esi, pv
			movups	xmm0, [esi]
			movaps	xmm2, xmm0

            mulps	xmm0, xmm0
            movaps	xmm1, xmm0
            movhlps xmm1, xmm1
            addps	xmm0, xmm1
            movaps	xmm1, xmm0
            shufps	xmm1, xmm1, 39h
            addps	xmm0, xmm1

			rsqrtps	xmm0, xmm0
            shufps  xmm0, xmm0, 00h
			mulps	xmm2, xmm0
			movups	[ecx], xmm2
		}

#elif PLATFORM == PLATFORM_LINUX
		length = sqrtf(x*x + y*y + z*z);
        if (length != 0.0f) {
            result.x = x / length;
            result.y = y / length;
            result.z = z / length;
        }
#endif
    }

    return result;
}

void Vector4::Normalize()
{
    float length;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        length = Length();
        if (length != 0.0f) {
            x /= length;
            y /= length;
            z /= length;
        }
    } else {
#if	PLATFORM == PLATFORM_WIN32
		Vector4 v(x, y, z);
		v.w = 0.0;
		Vector4* pv = &v;

		__asm {
			mov		esi, pv
            movups	xmm0, [esi]
            movaps	xmm2, xmm0

            mulps	xmm0, xmm0
            movaps	xmm1, xmm0
            movhlps xmm1, xmm1
            addps	xmm0, xmm1
            movaps	xmm1, xmm0
            shufps	xmm1, xmm1, 39h
            addps	xmm0, xmm1

            rsqrtps	xmm0, xmm0
            shufps  xmm0, xmm0, 00h
            mulps	xmm2, xmm0
            movups	[esi], xmm2
		}
		v.w = 1.0f;
		(*this) = v;

#elif PLATFORM == PLATFORM_LINUX
		length = sqrtf(x*x + y*y + z*z);
        if (length != 0.0f) {
            x /= length;
            y /= length;
            z /= length;
        }
#endif
    }
}

float Vector4::Dot(const Vector4& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

Vector4 Vector4::Cross(const Vector4& v) const
{
    return Vector4(y*v.z - z*v.y,
                   z*v.x - x*v.z,
                   x*v.y - y*v.x);
}

Vector4 Vector4::operator*(const Matrix4x4& m) const
{
    Vector4 v;

    v.x = x * m[0][0] + y * m[1][0] + z * m[2][0] + w * m[3][0];
    v.y = x * m[0][1] + y * m[1][1] + z * m[2][1] + w * m[3][1];
    v.z = x * m[0][2] + y * m[1][2] + z * m[2][2] + w * m[3][2];
    v.w = x * m[0][3] + y * m[1][3] + z * m[2][3] + w * m[3][3];

    return v;
}

void Vector4::operator*=(const Matrix4x4& m)
{
    float nx, ny, nz, nw;

    nx = x * m[0][0] + y * m[1][0] + z * m[2][0] + w * m[3][0];
    ny = x * m[0][1] + y * m[1][1] + z * m[2][1] + w * m[3][1];
    nz = x * m[0][2] + y * m[1][2] + z * m[2][2] + w * m[3][2];
    nw = x * m[0][3] + y * m[1][3] + z * m[2][3] + w * m[3][3];

    x = nx;
    y = ny;
    z = nz;
    w = nw;
}

}

