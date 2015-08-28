#ifndef SKETCH_3D_BUFFER_OPENGL_H
#define SKETCH_3D_BUFFER_OPENGL_H

#include "render/Buffer.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {

class SKETCH_3D_API BufferOpenGL
{
    public:
                                BufferOpenGL();
        virtual                ~BufferOpenGL();

        GLuint                  GetBuffer() const;

    protected:
        GLuint                  buffer_;
        size_t                  bufferSizeInBytes_;
        void                    InternalBindBuffer(GLenum target) const;
        void*                   InternalMap(GLenum mapFlag, GLenum target) const;
        void                    InternalUnmap(GLenum target) const;
};

class SKETCH_3D_API VertexBufferOpengl : public VertexBuffer, public BufferOpenGL
{
    public:
        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

class SKETCH_3D_API IndexBufferOpenGL : public IndexBuffer, public BufferOpenGL
{
    public:
        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

class SKETCH_3D_API ConstantBufferOpenGL : public ConstantBuffer, public BufferOpenGL
{
    public:
        virtual bool    Initialize(void* initialData, bool dynamic, bool immutable, size_t dataSize) override;
        virtual void*   Map(MapFlag_t mapFlag) const override;
        virtual void    Unmap() const override;
};

GLenum GetOGLMapFlag(MapFlag_t mapFlag);

}

#endif