#include "render/Direct3D11/BufferDirect3D11.h"

#include "render/Direct3D11/RenderDeviceDirect3D11.h"

#include "system/Logger.h"

namespace Sketch3D {
BufferDirect3D11::BufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context) : device_(device), context_(context), buffer_(nullptr) {
}

BufferDirect3D11::~BufferDirect3D11() {
    if (buffer_ != nullptr) {
        buffer_->Release();
    }
}

ID3D11Buffer* const BufferDirect3D11::GetBuffer() const {
    return buffer_;
}

void* BufferDirect3D11::InternalMap(D3D11_MAP mapFlag) const {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context_->Map(buffer_, 0, mapFlag, 0, &mappedResource);
    if (FAILED(hr)) {
        return nullptr;
    }

    return mappedResource.pData;
}

void BufferDirect3D11::InternalUnmap() const {
    context_->Unmap(buffer_, 0);
}

VertexBufferDirect3D11::VertexBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context) : BufferDirect3D11(device, context) {
}

bool VertexBufferDirect3D11::Initialize(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices) {
    if (buffer_ != nullptr) {
        Logger::GetInstance()->Warning("Vertex buffer with resource id # " + to_string(resourceId_) + " already created");
        return false;
    }

    vertexFormat_ = vertexFormat;
    numVertices_ = numVertices;

    D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
    if (immutable) {
        usage = D3D11_USAGE_IMMUTABLE;
    } else if (dynamic) {
        usage = D3D11_USAGE_DYNAMIC;
    }

    D3D11_BUFFER_DESC desc;
    desc.Usage = usage;
    desc.ByteWidth = vertexFormat->GetSize() * numVertices;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = (dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = initialData;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT result = device_->CreateBuffer(&desc, &data, &buffer_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create vertex buffer");
        return false;
    }

    return true;
}

void* VertexBufferDirect3D11::Map(MapFlag_t mapFlag) const {
    return InternalMap(GetD3DMapFlag(mapFlag));
}

void VertexBufferDirect3D11::Unmap() const {
    InternalUnmap();
}

IndexBufferDirect3D11::IndexBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context) : BufferDirect3D11(device, context) {
}

bool IndexBufferDirect3D11::Initialize(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) {
    if (buffer_ != nullptr) {
        Logger::GetInstance()->Warning("Index buffer with resource id # " + to_string(resourceId_) + " already created");
        return false;
    }

    indexFormat_ = indexFormat;
    numIndices_ = numIndices;

    D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
    if (immutable) {
        usage = D3D11_USAGE_IMMUTABLE;
    } else if (dynamic) {
        usage = D3D11_USAGE_DYNAMIC;
    }

    size_t indexSize = (indexFormat == IndexFormat_t::INT_2_BYTES) ? sizeof(unsigned short) : sizeof(unsigned int);

    D3D11_BUFFER_DESC desc;
    desc.Usage = usage;
    desc.ByteWidth = numIndices * indexSize;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = (dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = initialData;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT result = device_->CreateBuffer(&desc, &data, &buffer_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create index buffer");
        return false;
    }

    return true;
}

void* IndexBufferDirect3D11::Map(MapFlag_t mapFlag) const {
    return InternalMap(GetD3DMapFlag(mapFlag));
}

void IndexBufferDirect3D11::Unmap() const {
    InternalUnmap();
}

ConstantBufferDirect3D11::ConstantBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context) : BufferDirect3D11(device, context) {
}

bool ConstantBufferDirect3D11::Initialize(void* initialData, bool dynamic, bool immutable, size_t dataSize) {
    if (buffer_ != nullptr) {
        Logger::GetInstance()->Warning("Constant buffer with resource id # " + to_string(resourceId_) + " already created");
        return false;
    }

    D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
    if (immutable) {
        usage = D3D11_USAGE_IMMUTABLE;
    } else if (dynamic) {
        usage = D3D11_USAGE_DYNAMIC;
    }

    size_t sizeRoundedTo16Bytes = ((dataSize + 15) / 16) * 16;
    if (sizeRoundedTo16Bytes != dataSize) {
        Logger::GetInstance()->Error("Constant buffer structure size isn't rounded to 16 bytes boundary");
        return false;
    }

    D3D11_BUFFER_DESC desc;
    desc.Usage = usage;
    desc.ByteWidth = dataSize;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = (dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    D3D11_SUBRESOURCE_DATA* pData = &data;

    if (initialData != nullptr) {
        pData->pSysMem = initialData;
        pData->SysMemPitch = 0;
        pData->SysMemSlicePitch = 0;
    } else {
        pData = nullptr;
    }

    HRESULT result = device_->CreateBuffer(&desc, pData, &buffer_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create constant buffer");
        return false;
    }

    return true;
}

void* ConstantBufferDirect3D11::Map(MapFlag_t mapFlag) const {
    return InternalMap(GetD3DMapFlag(mapFlag));
}

void ConstantBufferDirect3D11::Unmap() const {
    InternalUnmap();
}

D3D11_MAP GetD3DMapFlag(MapFlag_t mapFlag) {
    D3D11_MAP mapType;
    switch (mapFlag) {
        case MapFlag_t::READ: mapType = D3D11_MAP_READ; break;
        case MapFlag_t::READ_WRITE: mapType = D3D11_MAP_READ_WRITE; break;
        case MapFlag_t::WRITE: mapType = D3D11_MAP_WRITE; break;
        case MapFlag_t::WRITE_DISCARD: mapType = D3D11_MAP_WRITE_DISCARD; break;
        case MapFlag_t::WRITE_NO_OVERWRITE: mapType = D3D11_MAP_WRITE_NO_OVERWRITE; break;
    }

    return mapType;
}

}