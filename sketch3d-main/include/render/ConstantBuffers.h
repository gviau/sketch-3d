#ifndef SKETCH_3D_CONSTANT_BUFFERS_H
#define SKETCH_3D_CONSTANT_BUFFERS_H

#include "system/Platform.h"

#include "math/Matrix4x4.h"
#include "math/Vector4.h"

namespace Sketch3D {
// Constant buffers should be aligned to 16 bytes boundaries for performance reason
struct ALIGNED_(16) PassConstants_t {
    Matrix4x4 projectionMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 modelMatrix;

    Matrix4x4 modelViewProjectionMatrix;
    Matrix4x4 ViewProjectionMatrix;
    Matrix4x4 modelViewMatrix;

    Matrix4x4 inverseTransposeModelViewMatrix;
};

}

#endif