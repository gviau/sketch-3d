#include "render/Direct3D9/BufferObjectDirect3D9.h"

#include "math/Vector2.h"
#include "math/Vector3.h"

#include <vector>
using namespace std;

#include <d3d9.h>

namespace Sketch3D {
BufferObjectDirect3D9::BufferObjectDirect3D9(IDirect3DDevice9* device, const VertexAttributesMap_t& vertexAttributes,
                                             BufferUsage_t usage) : BufferObject(vertexAttributes, usage), device_(device),
                                             vertexBuffer_(nullptr), indexBuffer_(nullptr), vertexDeclaration_(nullptr), stride_(0), primitivesCount_(0)
{
}

BufferObjectDirect3D9::~BufferObjectDirect3D9() {
    if (vertexBuffer_ != nullptr) {
        vertexBuffer_->Release();
    }

    if (indexBuffer_ != nullptr) {
        indexBuffer_->Release();
    }

    if (vertexDeclaration_ != nullptr) {
        vertexDeclaration_->Release();
    }
}

void BufferObjectDirect3D9::Render() {
    device_->SetVertexDeclaration(vertexDeclaration_);
    device_->SetStreamSource(0, vertexBuffer_, 0, stride_);
    device_->SetIndices(indexBuffer_);
    device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount_, 0, primitivesCount_);
}

bool BufferObjectDirect3D9::SetVertexData(const vector<float>& vertexData, int presentVertexAttributes) {
    // We implicitely count position
    size_t count = 1;
    VertexAttributesMap_t::iterator it = vertexAttributes_.begin();

    for (; it != vertexAttributes_.end(); ++it) {
        if ((presentVertexAttributes & it->first) > 0) {
            count += 1;
        }
    }

    if (count != vertexAttributes_.size()) {
        return false;
    }

    if (vertexDeclaration_ != nullptr) {
        vertexDeclaration_->Release();
        vertexDeclaration_ = nullptr;
    }

    map<size_t, VertexAttributes_t> attributesFromIndex;
    it = vertexAttributes_.begin();
    for (; it != vertexAttributes_.end(); ++it) {
        attributesFromIndex[it->second] = it->first;
    }

    vector<D3DVERTEXELEMENT9> vertexElements;
    stride_ = 0;

    map<size_t, VertexAttributes_t>::iterator v_it = attributesFromIndex.begin();
    for (; v_it != attributesFromIndex.end(); ++v_it) {
        size_t offset = 0;
        _D3DDECLTYPE type;
        _D3DDECLUSAGE usage;

        switch (v_it->second) {
            case VERTEX_ATTRIBUTES_POSITION:
                offset = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_POSITION;
                break;

            case VERTEX_ATTRIBUTES_NORMAL:
                offset = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_NORMAL;
                break;

            case VERTEX_ATTRIBUTES_TEX_COORDS:
                offset = sizeof(Vector2);
                type = D3DDECLTYPE_FLOAT2;
                usage = D3DDECLUSAGE_TEXCOORD;
                break;

            case VERTEX_ATTRIBUTES_TANGENT:
                offset = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_TANGENT;
                break;
        }

        D3DVERTEXELEMENT9 vertexElement;
        vertexElement.Stream = 0;
        vertexElement.Offset = stride_;
        vertexElement.Type = type;
        vertexElement.Usage = usage;
        vertexElement.Method = D3DDECLMETHOD_DEFAULT;
        vertexElement.UsageIndex = 0;
        vertexElements.push_back(vertexElement);

        stride_ += offset;
    }

    D3DVERTEXELEMENT9 endVertexElement = D3DDECL_END();
    vertexElements.push_back(endVertexElement);

    device_->CreateVertexDeclaration(&vertexElements[0], &vertexDeclaration_);

    vertexCount_ = vertexData.size() / (stride_ / sizeof(float));

    if (vertexBuffer_ != nullptr) {
        vertexBuffer_->Release();
        vertexBuffer_ = nullptr;
    }

    DWORD usage = (usage_ == BUFFER_USAGE_STATIC) ? D3DUSAGE_WRITEONLY : D3DUSAGE_DYNAMIC;
    device_->CreateVertexBuffer(vertexCount_ * stride_, usage, 0, D3DPOOL_MANAGED, &vertexBuffer_, nullptr);

    void* data;
    size_t bufferSize = vertexData.size() * sizeof(float);

    DWORD lockFlags = (usage_ == BUFFER_USAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0;
    vertexBuffer_->Lock(0, bufferSize, &data, lockFlags);

    memcpy(data, (void*)&vertexData[0], bufferSize);

    vertexBuffer_->Unlock();

    return true;
}

bool BufferObjectDirect3D9::AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes) {
    return false;
}

void BufferObjectDirect3D9::SetIndexData(unsigned short* indexData, size_t numIndex) {
    if (indexBuffer_ != nullptr) {
        indexBuffer_->Release();
        indexBuffer_ = nullptr;
    }
    device_->CreateIndexBuffer(numIndex * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer_, nullptr);

    indexCount_ = numIndex;

    void* data;
    DWORD lockFlags = 0;
    indexBuffer_->Lock(0, indexCount_ * sizeof(unsigned short), &data, lockFlags);

    memcpy(data, (void*)indexData, indexCount_ * sizeof(unsigned short));

    indexBuffer_->Unlock();

    primitivesCount_ = numIndex / 3;
}

void BufferObjectDirect3D9::AppendIndexData(unsigned short* indexData, size_t numIndex) {
}

void BufferObjectDirect3D9::GenerateBuffers() {
    if (vertexBuffer_ == nullptr) {
        DWORD usage = (usage_ == BUFFER_USAGE_STATIC) ? D3DUSAGE_WRITEONLY : D3DUSAGE_DYNAMIC;
        device_->CreateVertexBuffer(0, usage, 0, D3DPOOL_MANAGED, &vertexBuffer_, nullptr);
    }

    if (indexBuffer_ == nullptr) {
        device_->CreateIndexBuffer(0, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer_, nullptr);
    }
}

}