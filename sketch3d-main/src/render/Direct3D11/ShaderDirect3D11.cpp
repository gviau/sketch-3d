#include "render/Direct3D11/ShaderDirect3D11.h"

#include "render/RenderDevice_Common.h"
#include "render/VertexFormat.h"

#include "system/Logger.h"

#include <vector>
using namespace std;

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
ShaderDirect3D11::ShaderDirect3D11(ID3D11Device* device)
    : device_(device)
    , shaderBlob_(nullptr)
{
}

ShaderDirect3D11::~ShaderDirect3D11() {
    if (shaderBlob_ != nullptr) {
        shaderBlob_->Release();
    }
}

ID3D10Blob* ShaderDirect3D11::GetShaderBlob() const {
    return shaderBlob_;
}

FragmentShaderDirect3D11::FragmentShaderDirect3D11(ID3D11Device* device)
    : ShaderDirect3D11(device)
    , shader_(nullptr)
{
}

FragmentShaderDirect3D11::~FragmentShaderDirect3D11() {
    if (shader_ != nullptr) {
        shader_->Release();
    }
}

bool FragmentShaderDirect3D11::InitializeFromSource(const string& source) {
    ID3D10Blob* error = nullptr;

    D3D_FEATURE_LEVEL featureLevel = device_->GetFeatureLevel();
    string shaderVersion = GetD3DShaderVersion(featureLevel);

    string version = ("ps_" + shaderVersion);
    unsigned int flags = 0;

#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, nullptr, "main", version.c_str(), flags, 0, &shaderBlob_, &error);
    if (FAILED(hr)) {
        if (error != nullptr) {
            char* errorMsg = (char*)error->GetBufferPointer();
            Logger::GetInstance()->Error("Couldn't create Fragment Shader: " + string(errorMsg, error->GetBufferSize()));
        }

        return false;
    }

    hr = device_->CreatePixelShader(shaderBlob_->GetBufferPointer(), shaderBlob_->GetBufferSize(), nullptr, &shader_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Pixel Shader creation fialed");
        return false;
    }

    return true;
}

ID3D11PixelShader* FragmentShaderDirect3D11::GetShader() const {
    return shader_;
}

VertexShaderDirect3D11::VertexShaderDirect3D11(ID3D11Device* device)
    : ShaderDirect3D11(device)
    , shader_(nullptr)
{
}

VertexShaderDirect3D11::~VertexShaderDirect3D11() {
    if (shader_ != nullptr) {
        shader_->Release();
    }
}

bool VertexShaderDirect3D11::InitializeFromSource(const string& source) {
    ID3D10Blob* error = nullptr;

    D3D_FEATURE_LEVEL featureLevel = device_->GetFeatureLevel();
    string shaderVersion = GetD3DShaderVersion(featureLevel);

    string version = ("vs_" + shaderVersion);
    unsigned int flags = 0;

#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, nullptr, "main", version.c_str(), flags, 0, &shaderBlob_, &error);
    if (FAILED(hr)) {
        if (error != nullptr) {
            char* errorMsg = (char*)error->GetBufferPointer();
            Logger::GetInstance()->Error("Couldn't create Vertex Shader: " + string(errorMsg, error->GetBufferSize()));
            error->Release();
        }

        return false;
    }

    bool success = true;
    hr = device_->CreateVertexShader(shaderBlob_->GetBufferPointer(), shaderBlob_->GetBufferSize(), nullptr, &shader_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Vertex Shader creation fialed");
        success = false;
    }
    
    if (error != nullptr) {
        error->Release();
    }

    return success;
}

ID3D11VertexShader* VertexShaderDirect3D11::GetShader() const {
    return shader_;
}

string GetD3DShaderVersion(D3D_FEATURE_LEVEL featureLevel) {
    string version = "";
    switch (featureLevel) {
    case D3D_FEATURE_LEVEL_11_0:
        version = "5_0";
        break;

    case D3D_FEATURE_LEVEL_10_1:
        version = "4_1";
        break;

    case D3D_FEATURE_LEVEL_10_0:
        version = "4_0";
        break;

    case D3D_FEATURE_LEVEL_9_3:
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
        version = "3_0";
        break;
    }

    return version;
}

LPCSTR GetD3DSemanticName(SemanticName_t semanticName) {
    LPCSTR name;
    switch (semanticName) {
        case SemanticName_t::BINORMAL:  name = "BINORMAL"; break;
        case SemanticName_t::BONES:     name = "BLENDINDICES"; break;
        case SemanticName_t::COLOR:     name = "COLOR"; break;
        case SemanticName_t::NORMAL:    name = "NORMAL"; break;
        case SemanticName_t::POSITION:  name = "POSITION"; break;
        case SemanticName_t::TANGENT:   name = "TANGENT"; break;
        case SemanticName_t::TEXCOORD:  name = "TEXCOORD"; break;
        case SemanticName_t::WEIGHTS:   name = "BLENDWEIGHT"; break;
    }

    return name;
}

DXGI_FORMAT GetD3DFormat(InputFormat_t inputFormat) {
    DXGI_FORMAT format;
    switch (inputFormat) {
        case InputFormat_t::BYTE_UINT1:         format = DXGI_FORMAT_R8_UINT; break;
        case InputFormat_t::BYTE_UINT2:         format = DXGI_FORMAT_R8G8_UINT; break;
        case InputFormat_t::BYTE_UINT4:         format = DXGI_FORMAT_R8G8B8A8_UINT; break;

        case InputFormat_t::BYTE_SINT1:         format = DXGI_FORMAT_R8_SINT; break;
        case InputFormat_t::BYTE_SINT2:         format = DXGI_FORMAT_R8G8_SINT; break;
        case InputFormat_t::BYTE_SINT4:         format = DXGI_FORMAT_R8G8B8A8_SINT; break;

        case InputFormat_t::HALF_FLOAT1:        format = DXGI_FORMAT_R16_FLOAT; break;
        case InputFormat_t::HALF_FLOAT2:        format = DXGI_FORMAT_R16G16_FLOAT; break;
        case InputFormat_t::HALF_FLOAT4:        format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;

        case InputFormat_t::HALF_UINT1:         format = DXGI_FORMAT_R16_UINT; break;
        case InputFormat_t::HALF_UINT2:         format = DXGI_FORMAT_R16G16_UINT; break;
        case InputFormat_t::HALF_UINT4:         format = DXGI_FORMAT_R16G16B16A16_UINT; break;

        case InputFormat_t::HALF_SINT1:         format = DXGI_FORMAT_R16_SINT; break;
        case InputFormat_t::HALF_SINT2:         format = DXGI_FORMAT_R16G16_SINT; break;
        case InputFormat_t::HALF_SINT4:         format = DXGI_FORMAT_R16G16B16A16_SINT; break;

        case InputFormat_t::FLOAT1:             format = DXGI_FORMAT_R32_FLOAT; break;
        case InputFormat_t::FLOAT2:             format = DXGI_FORMAT_R32G32_FLOAT; break;
        case InputFormat_t::FLOAT3:             format = DXGI_FORMAT_R32G32B32_FLOAT; break;
        case InputFormat_t::FLOAT4:             format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;

        case InputFormat_t::UINT1:              format = DXGI_FORMAT_R32_UINT; break;
        case InputFormat_t::UINT2:              format = DXGI_FORMAT_R32G32_UINT; break;
        case InputFormat_t::UINT3:              format = DXGI_FORMAT_R32G32B32_UINT; break;
        case InputFormat_t::UINT4:              format = DXGI_FORMAT_R32G32B32A32_UINT; break;

        case InputFormat_t::SINT1:              format = DXGI_FORMAT_R32_SINT; break;
        case InputFormat_t::SINT2:              format = DXGI_FORMAT_R32G32_SINT; break;
        case InputFormat_t::SINT3:              format = DXGI_FORMAT_R32G32B32_SINT; break;
        case InputFormat_t::SINT4:              format = DXGI_FORMAT_R32G32B32A32_SINT; break;

        case InputFormat_t::R10G10B10A2_UINT:   format = DXGI_FORMAT_R10G10B10A2_UINT; break;
        case InputFormat_t::R11G11B10_FLOAT:    format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
    }

    return format;
}

}