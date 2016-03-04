#ifndef SKETCH_3D_FORWARD_RENDERING_MATERIAL_CODE_GENERATOR_H
#define SKETCH_3D_FORWARD_RENDERING_MATERIAL_CODE_GENERATOR_H

#include "framework/MaterialCodeGenerator.h"

namespace Sketch3D
{
class ForwardRenderingMaterialCodeGenerator : public MaterialCodeGenerator
{
protected:
    virtual void    WriteVertexShaderMainBody(string& shaderCode);
    virtual void    WriteFragmentShaderMainBody(string& shaderCode);
};
}

#endif