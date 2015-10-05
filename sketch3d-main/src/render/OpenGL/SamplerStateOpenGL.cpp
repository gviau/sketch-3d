#include "render/OpenGL/SamplerStateOpenGL.h"

namespace Sketch3D
{
bool SamplerStateOpenGL::Initialize(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV, AddressMode_t addressModeW,
                                    ComparisonFunction_t comparisonFunction, const Vector4& borderColor)
{
    filterMode_ = filterMode;
    addressModeU_ = addressModeU;
    addressModeV_ = addressModeV;
    addressModeW_ = addressModeW;
    comparisonFunction_ = comparisonFunction;
    borderColor_ = borderColor;

    return true;
}

GLint GetOGLFilterMode(FilterMode_t filterMode)
{
    GLint mode;

    switch (filterMode)
    {
    case FilterMode_t::NEAREST: mode = GL_NEAREST; break;
    case FilterMode_t::LINEAR:  mode = GL_LINEAR; break;
    }

    return mode;
}

GLint GetOGLAddressMode(AddressMode_t addressMode)
{
    GLint address;

    switch (addressMode)
    {
    case AddressMode_t::BORDER: address = GL_CLAMP_TO_BORDER; break;
    case AddressMode_t::CLAMP:  address = GL_CLAMP; break;
    case AddressMode_t::MIRROR: address = GL_MIRRORED_REPEAT; break;
    case AddressMode_t::WRAP:   address = GL_REPEAT; break;
    }

    return address;
}

GLint GetOGLComparisonFunction(ComparisonFunction_t comparisonFunction)
{
    GLint comparison;
    switch (comparisonFunction) {
        case ComparisonFunction_t::ALWAYS:          comparison = GL_ALWAYS; break;
        case ComparisonFunction_t::EQUAL:           comparison = GL_EQUAL; break;
        case ComparisonFunction_t::GREATER:         comparison = GL_GREATER; break;
        case ComparisonFunction_t::GREATER_EQUAL:   comparison = GL_GEQUAL; break;
        case ComparisonFunction_t::LESS:            comparison = GL_LESS; break;
        case ComparisonFunction_t::LESS_EQUAL:      comparison = GL_LEQUAL; break;
        case ComparisonFunction_t::NEVER:           comparison = GL_NEVER; break;
        case ComparisonFunction_t::NOT_EQUAL:       comparison = GL_NOTEQUAL; break;
    }

    return comparison;
}

}