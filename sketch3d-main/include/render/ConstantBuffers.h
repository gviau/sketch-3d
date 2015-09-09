#ifndef SKETCH_3D_CONSTANT_BUFFERS_H
#define SKETCH_3D_CONSTANT_BUFFERS_H

#include "system/Platform.h"

#include "math/Matrix4x4.h"
#include "math/Vector4.h"

namespace Sketch3D {

/**
 * @class GPUMatrix4x4
 * This class is a class that takes a 4x4 matrix as input and automatically transpose it, to avoid programmer errors
 */
class SKETCH_3D_API GPUMatrix4x4
{
    public:
        GPUMatrix4x4(const Matrix4x4& src) : data_(src.Transpose()) {}
        void operator= (const Matrix4x4& rhs) { data_ = rhs.Transpose(); }

    private:
        Matrix4x4 data_;
};

// Constant buffers should be aligned to 16 bytes boundaries for performance reason
struct ALIGNED_(16) PassConstants_t {
    GPUMatrix4x4 projectionMatrix;
    GPUMatrix4x4 viewMatrix;
};

struct ALIGNED_(16) DrawConstants_t {
    GPUMatrix4x4 modelMatrix;
    GPUMatrix4x4 modelViewProjectionMatrix;
    GPUMatrix4x4 ViewProjectionMatrix;
    GPUMatrix4x4 modelViewMatrix;

    GPUMatrix4x4 inverseTransposeModelViewMatrix;
};

struct ALIGNED_(16) ShadowConstants_t {
    GPUMatrix4x4 lightViewMatrix;
    GPUMatrix4x4 lightProjectionMatrix;
};

}

#endif