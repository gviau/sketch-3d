#ifndef SKETCH_3D_MATERIAL_CODE_GENERATOR_H
#define SKETCH_3D_MATERIAL_CODE_GENERATOR_H

#include "system/Platform.h"

#include "render/VertexFormat.h"

#include <string>
using namespace std;

namespace Sketch3D
{
class MaterialCodeGenerator
{
public:
    void            WriteShader(VertexFormatType_t vertexFormatType, bool usesAmbientTexture, bool usesDiffuseTexture, bool usesSpecularTexture,
                                bool usesNormalTexture, string& vertexShaderCode, string& fragmentShaderCode);

protected:
    virtual void    WriteVertexShaderConstantBuffers(string& shaderCode);

    virtual void    WriteVertexShaderInputs(VertexFormatType_t vertexFormatType, string& shaderCode);
    virtual void    WriteVertexShaderOutputs(string& shaderCode);

    virtual void    WriteFragmentShaderConstantBuffers(string& fragmentShaderCode);

    virtual void    WriteFragmentShaderInputs(string& shaderCode);
    virtual void    WriteFragmentShaderOutputs(string& shaderCode);
        
    virtual void    WriteVertexShaderMainBody(string& shaderCode) = 0;
    virtual void    WriteFragmentShaderMainBody(string& shaderCode) = 0;

    virtual string  GetVertexShaderInputStructureName() { return "VS_INPUT"; }
    virtual string  GetVertexShaderOutputStructureName() { return "VS_OUTPUT"; }
    virtual string  GetVertexShaderInputStructureVariableName() { return "input"; }
    virtual string  GetVertexShaderOutputStructureVariableName() { return "output"; }

    virtual string  GetFragmentShaderInputStructureName() { return "PS_INPUT"; }
    virtual string  GetFragmentShaderOutputStructureName() { return "PS_OUTPUT"; }
    virtual string  GetFragmentShaderInputStructureVariableName() { return "input"; }
    virtual string  GetFragmentShaderOutputStructureVariableName() { return "output"; }

    string          GetTabs(int tabLevel) const;

    bool            m_HasColors;
    bool            m_HasNormals;
    bool            m_HasUVs;
    bool            m_HasTangents;
    bool            m_HasBones;
    bool            m_Has2UVs;

    bool            m_UsesAmbientTexture;
    bool            m_UsesDiffuseTexture;
    bool            m_UsesSpecularTexture;
    bool            m_UsesNormalTexture;
};
}

#endif