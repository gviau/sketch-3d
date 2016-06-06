#ifndef SKETCH_3D_CONSTANT_BUFFERS_H
#define SKETCH_3D_CONSTANT_BUFFERS_H

#include "system/Platform.h"

#include "math/Matrix4x4.h"
#include "math/Vector4.h"

namespace Sketch3D {

const int MAX_BONES = 128;
const int MAX_NUM_LIGHTS = 8;

/**
 * @class GPUMatrix4x4
 * This class is a class that takes a 4x4 matrix as input and automatically transpose it, to avoid programmer errors
 */
class SKETCH_3D_API GPUMatrix4x4
{
    public:
        GPUMatrix4x4() {}
        GPUMatrix4x4(const Matrix4x4& src) : data_(src.Transpose()) {}
        void operator= (const Matrix4x4& rhs) { data_ = rhs.Transpose(); }

    private:
        Matrix4x4 data_;
};

// Constant buffers should be aligned to 16 bytes boundaries for performance reason
struct ALIGNED_(16) PassConstants_t
{
    GPUMatrix4x4 projectionMatrix;
    GPUMatrix4x4 viewMatrix;
    GPUMatrix4x4 viewProjectionMatrix;
};

struct ALIGNED_(16) DrawConstants_t
{
    GPUMatrix4x4 modelMatrix;
    GPUMatrix4x4 modelViewProjectionMatrix;
    GPUMatrix4x4 modelViewMatrix;

    GPUMatrix4x4 transposedInverseModelViewMatrix;
};


struct ALIGNED_(16) BoneConstants_t
{
	GPUMatrix4x4 boneTransformationMatrices[MAX_BONES];
};

struct ALIGNED_(16) LightConstants_t
{
    int numLights;
    int pad[3];

    Vector4 lightPositions[MAX_NUM_LIGHTS];
    Vector4 lightDirections[MAX_NUM_LIGHTS];
    Vector4 lightColors[MAX_NUM_LIGHTS];
};

struct ALIGNED_(16) ShadowConstants_t
{
    GPUMatrix4x4 lightViewMatrix;
    GPUMatrix4x4 lightProjectionMatrix;
};

struct ALIGNED_(16) MaterialConstants_t
{
    Vector4 ambientColor;
    Vector4 diffuseColor;
    Vector4 specularColorAndPower;
};

}

#endif