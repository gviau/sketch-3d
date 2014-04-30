#include "math/Vector3.h"

#include "math/Constants.h"
#include "math/Matrix3x3.h"
#include "system/Platform.h"

#include <iostream>
#include <math.h>

namespace Sketch3D {

const Vector3 Vector3::ZERO (0.0f, 0.0f, 0.0f);
const Vector3 Vector3::ONE  (1.0f, 1.0f, 1.0f);
const Vector3 Vector3::RIGHT(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP   (0.0f, 1.0f, 0.0f);
const Vector3 Vector3::LOOK (0.0f, 0.0f, 1.0f);

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3::Vector3(const Vector3& src)
{
    x = src.x;
    y = src.y;
    z = src.z;
}

float Vector3::Length() const
{
    float f;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        f = sqrtf(x*x + y*y + z*z);
    } else {
#if	PLATFORM == PLATFORM_WIN32
		float* pf = &f;

		__asm {
			mov		ecx, pf
			mov		esi, this
			movups	xmm0, [esi]
			mulps	xmm0, xmm0
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 4Eh
			addps	xmm0, xmm1
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 11h
			addps	xmm0, xmm1
			sqrtss	xmm0, xmm0
			movss	[ecx], xmm0
		}
#elif PLATFORM == PLATFORM_LINUX
		f = sqrtf(x*x + y*y + z*z);
#endif
		/*
        __m128 u = {x, y, z, 0.0f};
        __m128 v;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);
        u = _mm_sqrt_ss(u);
        _mm_store_ss(&f, u);
		*/
    }
    
    return f;
}

float Vector3::SquaredLength() const
{
    return x*x + y*y + z*z;
}

Vector3 Vector3::Normalized() const
{
    Vector3 result;
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
		Vector3* pV = &result;

		__asm {
			mov		ecx, pV
			mov		esi, this
			movups	xmm0, [esi]
			movaps	xmm2, xmm0
			mulps	xmm0, xmm0
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 4Eh
			addps	xmm0, xmm1
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 11h
			addps	xmm0, xmm1

			rsqrtps	xmm0, xmm0
			mulps	xmm2, xmm0
			movups	[ecx], xmm2
		}
#elif PLATFORM == PLATFORM_LINUX
		f = sqrtf(x*x + y*y + z*z);
#endif
	/*
        __m128 u = {x, y, z, 0.0f};
        __m128 v;
        __m128 w = u;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);

        u = _mm_rsqrt_ps(u);
        w = _mm_mul_ps(u, w);

#if COMPILER == COMPILER_MSVC
        SIMD_ALIGNED_DECL(float, f[4]);
#elif COMPILER == COMPILER_GNUC
        float f[4];
#endif
        _mm_store_ps(f, w);

        result.x = f[0];
        result.y = f[1];
        result.z = f[2];
		*/
    }

    return result;
}

void Vector3::Normalize()
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
		__asm {
			mov		esi, this
			movups	xmm0, [esi]
			movaps	xmm2, xmm0
			mulps	xmm0, xmm0
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 4Eh
			addps	xmm0, xmm1
			movaps	xmm1, xmm0
			shufps	xmm1, xmm1, 11h
			addps	xmm0, xmm1

			rsqrtps	xmm0, xmm0
			mulps	xmm2, xmm0
			movups	[esi], xmm2
		}
#elif PLATFORM == PLATFORM_LINUX
		f = sqrtf(x*x + y*y + z*z);
#endif
/*
        __m128 u = {x, y, z, 0.0f};
        __m128 v;
        __m128 w = u;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);

        u = _mm_rsqrt_ps(u);
        w = _mm_mul_ps(u, w);

#if COMPILER == COMPILER_MSVC
        SIMD_ALIGNED_DECL(float, f[4]);
#elif COMPILER == COMPILER_GNUC
        float f[4];
#endif
        _mm_store_ps(f, w);

        x = f[0];
        y = f[1];
        z = f[2];
*/
    }
}

float Vector3::Dot(const Vector3& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const
{
    return Vector3(y*v.z - z*v.y,
                   z*v.x - x*v.z,
                   x*v.y - y*v.x);
}

Vector3 Vector3::operator*(const Matrix3x3& m) const
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    Vector3 v;

    v.x = x * m[0][0] + y * m[0][1] + z * m[0][2];
    v.y = x * m[1][0] + y * m[1][1] + z * m[1][2];
    v.z = x * m[2][0] + y * m[2][1] + z * m[2][2];

    return m * (*this);
}

void Vector3::operator*=(const Matrix3x3& m)
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    float nx, ny, nz;

    nx = x * m[0][0] + y * m[0][1] + z * m[0][2];
    ny = x * m[1][0] + y * m[1][1] + z * m[1][2];
    nz = x * m[2][0] + y * m[2][1] + z * m[2][2];

    x = nx;
    y = ny;
    z = nz;
}

}
