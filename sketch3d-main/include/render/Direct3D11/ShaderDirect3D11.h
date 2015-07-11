#ifndef SKETCH_3D_SHADER_DIRECT3D11_H
#define SKETCH_3D_SHADER_DIRECT3D11_H

#include "render/Shader.h"

#include "render/RenderDevice_Common.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
// Class forward declaration
class VertexFormat;

class SKETCH_3D_API ShaderDirect3D11 {
    public:
                        ShaderDirect3D11(ID3D11Device* device);

        ID3D10Blob*     GetShaderBlob() const;

    protected:
        ID3D11Device*   device_;
        ID3D10Blob*     shaderBlob_;
};

class SKETCH_3D_API FragmentShaderDirect3D11 : public FragmentShader, public ShaderDirect3D11 {
    public:
                            FragmentShaderDirect3D11(ID3D11Device* device);

        virtual bool        InitializeFromSource(const string& source) override;

        ID3D11PixelShader*  GetShader() const;

    private:
        ID3D11PixelShader*  shader_;
};

class SKETCH_3D_API VertexShaderDirect3D11 : public VertexShader, public ShaderDirect3D11 {
    public:
                            VertexShaderDirect3D11(ID3D11Device* device);

        virtual bool        InitializeFromSource(const string& source) override;
        virtual bool        CreateInputLayout(VertexFormat* vertexFormat) override;

        ID3D11VertexShader* GetShader() const;
        ID3D11InputLayout*  GetInputLayout() const;

    private:
        ID3D11VertexShader* shader_;
        ID3D11InputLayout*  inputLayout_;
};  

string GetD3DShaderVersion(D3D_FEATURE_LEVEL featureLevel);
LPCSTR GetD3DSemanticName(SemanticName_t semanticName);
DXGI_FORMAT GetD3DFormat(InputFormat_t inputFormat);

}

#endif