#include "render/Direct3D9/BufferObjectDirect3D9.h"

#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include <vector>
using namespace std;

#include <d3d9.h>

namespace Sketch3D {
BufferObjectDirect3D9::BufferObjectDirect3D9(IDirect3DDevice9* device, const VertexAttributesMap_t& vertexAttributes,
                                             BufferUsage_t usage) : BufferObject(vertexAttributes, usage), device_(device),
                                             vertexBuffer_(nullptr), indexBuffer_(nullptr), vertexDeclaration_(nullptr), primitivesCount_(0),
                                             instanceBuffer_(nullptr), instanceDataPrepared_(false)
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

    if (instanceBuffer_ != nullptr) {
        instanceBuffer_->Release();
    }
}

void BufferObjectDirect3D9::Render() {
    device_->SetVertexDeclaration(vertexDeclaration_);
    device_->SetStreamSource(0, vertexBuffer_, 0, stride_);
    device_->SetIndices(indexBuffer_);
    device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount_, 0, primitivesCount_);
}

void BufferObjectDirect3D9::RenderInstances(const vector<Matrix4x4>& modelMatrices) {
    if (instanceBuffer_ != nullptr) {
        instanceBuffer_->Release();
    }

    // We have to transpose back the matrices for Direct3D
    vector<Matrix4x4> tranposedModelMatrices;
    tranposedModelMatrices.resize(modelMatrices.size());
    for (size_t i = 0; i < modelMatrices.size(); i++) {
        tranposedModelMatrices[i] = modelMatrices[i].Transpose();
    }

    Matrix4x4* data = nullptr;
    device_->CreateVertexBuffer(modelMatrices.size() * sizeof(Matrix4x4), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &instanceBuffer_, nullptr);
    instanceBuffer_->Lock(0, 0, (void**)&data, 0);

    memcpy((void*)data, &tranposedModelMatrices[0], modelMatrices.size() * sizeof(Matrix4x4));

    instanceBuffer_->Unlock();

    device_->SetVertexDeclaration(vertexDeclaration_);
    device_->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | modelMatrices.size()));
    device_->SetStreamSource(0, vertexBuffer_, 0, stride_);
    device_->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
    device_->SetStreamSource(1, instanceBuffer_, 0, sizeof(Matrix4x4));
    device_->SetIndices(indexBuffer_);

    device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount_, 0, primitivesCount_);
}

BufferObjectError_t BufferObjectDirect3D9::SetVertexData(const vector<float>& vertexData, int presentVertexAttributes) {
    bool hasNormals = ((presentVertexAttributes & VERTEX_ATTRIBUTES_NORMAL) > 0);
    bool hasTexCoords = ((presentVertexAttributes & VERTEX_ATTRIBUTES_TEX_COORDS) > 0);
    bool hasTangents = ((presentVertexAttributes & VERTEX_ATTRIBUTES_TANGENT) > 0);
    bool hasBones = ((presentVertexAttributes & VERTEX_ATTRIBUTES_BONES) > 0);
    bool hasWeights = ((presentVertexAttributes & VERTEX_ATTRIBUTES_WEIGHTS) > 0);

    stride_ = sizeof(Vector3) +
                ((hasNormals) ? sizeof(Vector3) : 0) +
                ((hasTexCoords) ? sizeof(Vector2) : 0) +
                ((hasTangents) ? sizeof(Vector3) : 0) +
                ((hasBones) ? sizeof(Vector4) : 0) +
                ((hasWeights) ? sizeof(Vector4) : 0);

    if ( (vertexData.size() / (stride_ / sizeof(float))) > 65535) {
        return BUFFER_OBJECT_ERROR_NOT_ENOUGH_SPACE;
    } else if (!AreVertexAttributesValid(presentVertexAttributes)) {
        return BUFFER_OBJECT_ERROR_INVALID_VERTEX_ATTRIBUTES;
    }

    CreateVertexDeclaration(hasNormals, hasTexCoords, hasTangents, hasBones, hasWeights);

    size_t newVertexCount = vertexData.size() / (stride_ / sizeof(float));

    if (vertexBuffer_ != nullptr && newVertexCount != vertexCount_) {
        vertexBuffer_->Release();
        vertexBuffer_ = nullptr;
    }

    if (newVertexCount != vertexCount_) {
        DWORD usage = (usage_ == BUFFER_USAGE_STATIC) ? D3DUSAGE_WRITEONLY : D3DUSAGE_DYNAMIC;
        D3DPOOL pool = (usage_ == BUFFER_USAGE_STATIC) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;
        device_->CreateVertexBuffer(newVertexCount * stride_, usage, 0, pool, &vertexBuffer_, nullptr);
    }
    vertexCount_ = newVertexCount;

    void* data;
    size_t bufferSize = vertexData.size() * sizeof(float);

    DWORD lockFlags = (usage_ == BUFFER_USAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0;
    vertexBuffer_->Lock(0, bufferSize, &data, lockFlags);

    memcpy(data, (void*)&vertexData[0], bufferSize);

    vertexBuffer_->Unlock();

    return BUFFER_OBJECT_ERROR_NONE;
}

BufferObjectError_t BufferObjectDirect3D9::AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes) {
    if (vertexCount_ == 0) {
        return SetVertexData(vertexData, presentVertexAttributes);
    } else if ( (vertexCount_ + (vertexData.size() / (stride_ / sizeof(float))) ) > 65535) {
        return BUFFER_OBJECT_ERROR_NOT_ENOUGH_SPACE;
    }

    if (!AreVertexAttributesValid(presentVertexAttributes)) {
        return BUFFER_OBJECT_ERROR_INVALID_VERTEX_ATTRIBUTES;
    }

    // We have to copy the buffer that we have, reallocate the space for it, append the data and copy back the new array
    size_t arraySize = vertexCount_ * (stride_ / sizeof(float));
    size_t newSize = arraySize + vertexData.size();
    vector<float> newVertexData;
    newVertexData.resize(newSize);

    // Get the old data
    void* data;
    DWORD lockFlags = (usage_ == BUFFER_USAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0;
    vertexBuffer_->Lock(0, 0, &data, lockFlags);

    memcpy(&newVertexData[0], data, vertexCount_ * sizeof(float));

    vertexBuffer_->Unlock();

    // Append the new
    size_t idx = 0;
    for (size_t i = arraySize; i < newSize; i++) {
        newVertexData[i] = vertexData[idx++];
    }

    // Create a new vertex buffer and assign the new vertex data
    vertexCount_ = newSize / (stride_ / sizeof(float));
    vertexBuffer_->Release();
    DWORD usage = (usage_ == BUFFER_USAGE_STATIC) ? D3DUSAGE_WRITEONLY : D3DUSAGE_DYNAMIC;
    device_->CreateVertexBuffer(newSize * sizeof(float), usage, 0, D3DPOOL_MANAGED, &vertexBuffer_, nullptr);
    vertexBuffer_->Lock(0, 0, &data, lockFlags);

    memcpy(data, &newVertexData[0], newSize * sizeof(float));

    vertexBuffer_->Unlock();

    return BUFFER_OBJECT_ERROR_NONE;
}

BufferObjectError_t BufferObjectDirect3D9::SetIndexData(unsigned short* indexData, size_t numIndex) {
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

    return BUFFER_OBJECT_ERROR_NONE;
}

BufferObjectError_t BufferObjectDirect3D9::AppendIndexData(unsigned short* indexData, size_t numIndex) {
    if (indexCount_ == 0) {
        return SetIndexData(indexData, numIndex);
    }

    // We have to copy the buffer that we have, reallocate the space for it, append the data and copy back the new array
    size_t newSize = indexCount_ + numIndex;
    vector<unsigned int> newIndexData;
    newIndexData.resize(newSize);

    // Get the old data
    void* data;
    DWORD lockFlags = 0;
    indexBuffer_->Lock(0, 0, &data, lockFlags);

    memcpy(&newIndexData[0], data, indexCount_* sizeof(unsigned short));

    indexBuffer_->Unlock();

    // Append the new
    size_t idx = 0;
    for (size_t i = indexCount_; i < newSize; i++) {
        newIndexData[i] = indexData[idx++];
    }
    indexCount_ = newSize;

    // Create a new vertex buffer and assign the new vertex data
    primitivesCount_ = indexCount_ / 3;

    indexBuffer_->Release();
    device_->CreateIndexBuffer(numIndex * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer_, nullptr);
    indexBuffer_->Lock(0, indexCount_ * sizeof(unsigned short), &data, lockFlags);

    memcpy(data, (void*)&newIndexData[0], indexCount_ * sizeof(unsigned short));

    indexBuffer_->Unlock();

    return BUFFER_OBJECT_ERROR_NONE;
}

void BufferObjectDirect3D9::PrepareInstanceBuffers() {
    if (instanceDataPrepared_) {
        return;
    }

    // Size of buffer is somewhat arbitrary
    D3DVERTEXELEMENT9 elements[32];
    size_t numElements;
    vertexDeclaration_->GetDeclaration(elements, &numElements);

    // Ignore the last element because it is the END element
    numElements -= 1;
    int attributeLocation = -1;

    vector<D3DVERTEXELEMENT9> vertexElements;
    for (size_t i = 0; i < numElements; i++) {
        vertexElements.push_back(elements[i]);

        if (elements[i].Usage == D3DDECLUSAGE_TEXCOORD) {
            if (elements[i].UsageIndex > attributeLocation) {
                attributeLocation = elements[i].UsageIndex;
            }
        }
    }
    attributeLocation += 1;

    for (size_t i = 0; i < 4; i++) {
        D3DVERTEXELEMENT9 modelMatrixDecl;
        modelMatrixDecl.Stream = 1;
        modelMatrixDecl.Offset = sizeof(float) * i * 4;
        modelMatrixDecl.Type = D3DDECLTYPE_FLOAT4;
        modelMatrixDecl.Usage = D3DDECLUSAGE_TEXCOORD;
        modelMatrixDecl.Method = D3DDECLMETHOD_DEFAULT;
        modelMatrixDecl.UsageIndex = attributeLocation + i;

        vertexElements.push_back(modelMatrixDecl);
    }

    D3DVERTEXELEMENT9 endElement = D3DDECL_END();
    vertexElements.push_back(endElement);

    vertexDeclaration_->Release();
    device_->CreateVertexDeclaration(&vertexElements[0], &vertexDeclaration_);

    instanceDataPrepared_ = true;
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

void BufferObjectDirect3D9::CreateVertexDeclaration(bool hasNormals, bool hasTexCoords, bool hasTangents,
                                                    bool hasBones, bool hasWeights)
{
    if (vertexDeclaration_ == nullptr) {
        map<size_t, VertexAttributes_t> attributesFromIndex;
        VertexAttributesMap_t::iterator it = vertexAttributes_.begin();
        for (; it != vertexAttributes_.end(); ++it) {
            attributesFromIndex[it->second] = it->first;
        }

        vector<D3DVERTEXELEMENT9> vertexElements;
        map<size_t, VertexAttributes_t>::iterator v_it = attributesFromIndex.begin();
        size_t offset = 0;
        for (; v_it != attributesFromIndex.end(); ++v_it) {
            size_t size = 0;
            _D3DDECLTYPE type;
            _D3DDECLUSAGE usage;

            switch (v_it->second) {
            case VERTEX_ATTRIBUTES_POSITION:
                size = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_POSITION;
                break;

            case VERTEX_ATTRIBUTES_NORMAL:
                if (!hasNormals) {
                    continue;
                }

                size = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_NORMAL;
                break;

            case VERTEX_ATTRIBUTES_TEX_COORDS:
                if (!hasTexCoords) {
                    continue;
                }

                size = sizeof(Vector2);
                type = D3DDECLTYPE_FLOAT2;
                usage = D3DDECLUSAGE_TEXCOORD;
                break;

            case VERTEX_ATTRIBUTES_TANGENT:
                if (!hasTangents) {
                    continue;
                }

                size = sizeof(Vector3);
                type = D3DDECLTYPE_FLOAT3;
                usage = D3DDECLUSAGE_TANGENT;
                break;

            case VERTEX_ATTRIBUTES_BONES:
                if (!hasBones) {
                    continue;
                }

                size = sizeof(Vector4);
                type = D3DDECLTYPE_FLOAT4;
                usage = D3DDECLUSAGE_BLENDINDICES;
                break;

            case VERTEX_ATTRIBUTES_WEIGHTS:
                if (!hasWeights) {
                    continue;
                }

                size = sizeof(Vector4);
                type = D3DDECLTYPE_FLOAT4;
                usage = D3DDECLUSAGE_BLENDWEIGHT;
                break;
            }

            D3DVERTEXELEMENT9 vertexElement;
            vertexElement.Stream = 0;
            vertexElement.Offset = offset;
            vertexElement.Type = type;
            vertexElement.Usage = usage;
            vertexElement.Method = D3DDECLMETHOD_DEFAULT;
            vertexElement.UsageIndex = 0;
            vertexElements.push_back(vertexElement);

            offset += size;
        }

        D3DVERTEXELEMENT9 endVertexElement = D3DDECL_END();
        vertexElements.push_back(endVertexElement);

        device_->CreateVertexDeclaration(&vertexElements[0], &vertexDeclaration_);
    }
}

}