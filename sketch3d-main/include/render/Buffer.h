#ifndef SKETCH_3D_BUFFER_H
#define SKETCH_3D_BUFFER_H

#include "render/HardwareResource.h"

#include "render/VertexFormat.h"

namespace Sketch3D {
/**
 * @class Buffer
 * Base class for the different buffers
 */
class SKETCH_3D_API Buffer : public HardwareResource
{
    public:
        virtual        ~Buffer() {}

        virtual void*   Map(MapFlag_t mapFlag) const = 0;
        virtual void    Unmap() const = 0;
};

class SKETCH_3D_API VertexBuffer : public Buffer
{
    public:
        virtual bool        Initialize(void* initialData, bool dynamic, bool immutable, VertexFormatType_t vertexFormatType, size_t numVertices) = 0;
        VertexFormatType_t  GetVertexFormatType() const;
        size_t              GetNumVertices() const;

    protected:
        VertexFormatType_t  vertexFormatType_;
        size_t              numVertices_;
};

class SKETCH_3D_API IndexBuffer : public Buffer
{
    public:
        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) = 0;
        IndexFormat_t   GetIndexFormat() const;
        size_t          GetNumIndices() const;

    protected:
        IndexFormat_t   indexFormat_;
        size_t          numIndices_;
};

class SKETCH_3D_API ConstantBuffer : public Buffer
{
    public:
        virtual bool Initialize(void* initialData, bool dynamic, bool immutable, size_t dataSize) = 0;
};

}

#endif