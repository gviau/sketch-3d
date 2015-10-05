#ifndef SKETCH_3D_SAMPLER_STATE_OPENGL_H
#define SKETCH_3D_SAMPLER_STATE_OPENGL_H

#include "render/SamplerState.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {
class SKETCH_3D_API SamplerStateOpenGL : public SamplerState {
    public:
        virtual bool Initialize(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV, AddressMode_t addressModeW,
                                ComparisonFunction_t comparisonFunction, const Vector4& borderColor=Vector4::ZERO) override;
};

GLint GetOGLFilterMode(FilterMode_t filterMode);
GLint GetOGLAddressMode(AddressMode_t addressMode);
GLint GetOGLComparisonFunction(ComparisonFunction_t comparisonFunction);

}

#endif