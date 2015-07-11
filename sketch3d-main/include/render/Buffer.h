#ifndef SKETCH_3D_BUFFER_H
#define SKETCH_3D_BUFFER_H

#include "render/HardwareResource.h"

#include "render/VertexFormat.h"

namespace Sketch3D {
/**
 * @class Buffer
 * Base class for the different buffers
 */
class SKETCH_3D_API Buffer : public HardwareResource {
    public:
        /**
         * Constructor
         * @param initialData Pointer to the buffer's initial data
         * @param dynamic Is the buffer dynamic?
         * @param isImmutable Is the buffer immutable?
         */
                        Buffer(void* initialData, bool dynamic, bool immutable);

        virtual void*   Map(MapFlag_t mapFlag) const = 0;
        virtual void    Unmap() const = 0;
};

class SKETCH_3D_API VertexBuffer : public Buffer {
    public:
                        VertexBuffer(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices);

        VertexFormat*   GetVertexFormat() const;
        size_t          GetNumVertices() const;

    protected:
        VertexFormat*   vertexFormat_;
        size_t          numVertices_;
};

class SKETCH_3D_API IndexBuffer : public Buffer {
    public:
                        IndexBuffer(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices);

        IndexFormat_t   GetIndexFormat() const;
        size_t          GetNumIndices() const;

    protected:
        IndexFormat_t   indexFormat_;
        size_t          numIndices_;
};

class SKETCH_3D_API ConstantBuffer : public Buffer {
    public:
        ConstantBuffer(void* initialData, bool dynamic, bool immutable, size_t dataSize);
};

}

#endif