#include "render/Direct3D11/HardwareResourceCreatorDirect3D11.h"

#include "render/Direct3D11/BufferDirect3D11.h"
#include "render/Direct3D11/ShaderDirect3D11.h"
#include "render/Direct3D11/SamplerStateDirect3D11.h"
#include "render/Direct3D11/TextureDirect3D11.h"

#include "system/Logger.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
HardwareResourceCreatorDirect3D11::HardwareResourceCreatorDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context) : device_(device), context_(context) {
}

shared_ptr<ConstantBuffer> HardwareResourceCreatorDirect3D11::CreateConstantBuffer(void* initialData, bool dynamic, bool immutable, size_t dataSize)  {
    ConstantBuffer* constantBuffer = new ConstantBufferDirect3D11(device_, context_, initialData, dynamic, immutable, dataSize);
    return shared_ptr<ConstantBuffer>(constantBuffer);
}

shared_ptr<IndexBuffer> HardwareResourceCreatorDirect3D11::CreateIndexBuffer(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) {
    IndexBuffer* indexBuffer = new IndexBufferDirect3D11(device_, context_, initialData, dynamic, immutable, indexFormat, numIndices);
    return shared_ptr<IndexBuffer>(indexBuffer);
}

shared_ptr<FragmentShader> HardwareResourceCreatorDirect3D11::CreateFragmentShader() {
    FragmentShader* fragmentShader = new FragmentShaderDirect3D11(device_);
    return shared_ptr<FragmentShader>(fragmentShader);
}

shared_ptr<SamplerState> HardwareResourceCreatorDirect3D11::CreateSamplerState(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV,
        AddressMode_t addressModeW, ComparisonFunction_t comparisonFunction, const Vector4& borderColor)
{
    SamplerState* samplerState = new SamplerStateDirect3D11(device_, filterMode, addressModeU, addressModeV, addressModeW, comparisonFunction, borderColor);
    return shared_ptr<SamplerState>(samplerState);
}

shared_ptr<Texture2D> HardwareResourceCreatorDirect3D11::CreateTexture2D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) {
    if (textureMap == nullptr) {
        Logger::GetInstance()->Error("Invalid texture map passed for 2D texture creation");
        return nullptr;
    }

    Texture2D* texture2D = new Texture2DDirect3D11(device_, textureMap, textureFormat, dynamic, immutable);
    return shared_ptr<Texture2D>(texture2D);
}

shared_ptr<VertexBuffer> HardwareResourceCreatorDirect3D11::CreateVertexBuffer(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices) {
    VertexBuffer* vertexBuffer = new VertexBufferDirect3D11(device_, context_, initialData, dynamic, immutable, vertexFormat, numVertices);
    return shared_ptr<VertexBuffer>(vertexBuffer);
}

shared_ptr<VertexShader> HardwareResourceCreatorDirect3D11::CreateVertexShader() {
    VertexShader* vertexShader = new VertexShaderDirect3D11(device_);
    return shared_ptr<VertexShader>(vertexShader);
}
}