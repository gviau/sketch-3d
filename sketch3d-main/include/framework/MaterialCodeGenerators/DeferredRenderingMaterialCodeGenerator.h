#ifndef SKETCH_3D_DEFERRED_RENDERING_MATERIAL_CODE_GENERATOR_H
#define SKETCH_3D_DEFERRED_RENDERING_MATERIAL_CODE_GENERATOR_H

#include "framework/MaterialCodeGenerator.h"

namespace Sketch3D
{

class DeferredRenderingMaterialCodeGenerator : public MaterialCodeGenerator
{
protected:
    virtual void    WriteVertexShaderMainBody(string& shaderCode) override;
    virtual void    WriteFragmentShaderMainBody(string& shaderCode) override;

    virtual void    WriteFragmentShaderConstantBuffers(string& shaderCode) override;
    virtual void    WriteFragmentShaderOutputs(string& shaderCode) override;
};

}

#endif