#ifndef SKETCH_3D_BUFFER_DIRECT3D11_H
#define SKETCH_3D_BUFFER_DIRECT3D11_H

#include "render/Buffer.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {

class SKETCH_3D_API BufferDirect3D11
{
    public:
                                BufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);
        virtual                ~BufferDirect3D11();

        ID3D11Buffer* const     GetBuffer() const;

    protected:
        ID3D11Device*           device_;
        ID3D11DeviceContext*    context_;
        ID3D11Buffer*           buffer_;
        void*                   InternalMap(D3D11_MAP mapFlag) const;
        void                    InternalUnmap() const;
};

class SKETCH_3D_API VertexBufferDirect3D11 : public VertexBuffer, public BufferDirect3D11
{
    public:
                        VertexBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);

        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, VertexFormatType_t vertexFormatType, size_t numVertices) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

class SKETCH_3D_API IndexBufferDirect3D11 : public IndexBuffer, public BufferDirect3D11
{
    public:
                        IndexBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);

        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

class SKETCH_3D_API ConstantBufferDirect3D11 : public ConstantBuffer, public BufferDirect3D11
{
    public:
                        ConstantBufferDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);

        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, size_t dataSize) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

D3D11_MAP GetD3DMapFlag(MapFlag_t mapFlag);

}

#endif