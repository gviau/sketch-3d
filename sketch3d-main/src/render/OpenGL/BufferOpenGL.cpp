#include "render/OpenGL/BufferOpenGL.h"

#include "render/VertexFormat.h"

#include "system/Logger.h"

namespace Sketch3D {
BufferOpenGL::BufferOpenGL()
    : buffer_(0)
    , bufferSizeInBytes_(0)
{
}

BufferOpenGL::~BufferOpenGL()
{
    if (buffer_ != 0)
    {
        glDeleteBuffers(1, &buffer_);
    }
}

GLuint BufferOpenGL::GetBuffer() const
{
    return buffer_;
}

size_t BufferOpenGL::GetBufferSizeInBytes() const
{
    return bufferSizeInBytes_;
}

void* BufferOpenGL::InternalMap(GLenum mapFlag, GLenum target) const
{
    if (bufferSizeInBytes_ == 0)
    {
        return nullptr;
    }

    glBindBuffer(target, buffer_);
    void* data = glMapBufferRange(target, 0, bufferSizeInBytes_, mapFlag);

    return data;
}

void BufferOpenGL::InternalUnmap(GLenum target) const
{
    if (bufferSizeInBytes_ > 0)
    {
        glBindBuffer(target, buffer_);
        glFlushMappedBufferRange(target, 0, bufferSizeInBytes_);
        glUnmapBuffer(target);
    }
}

bool VertexBufferOpenGL::Initialize(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices)
{
    if (buffer_ > 0)
    {
        Logger::GetInstance()->Warning("Trying to initialize an already initialized vertex buffer");
        return false;
    }

    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);

    vertexFormat_ = vertexFormat;
    numVertices_ = numVertices;
    
    bufferSizeInBytes_ = numVertices_ * vertexFormat_->GetSize();

    GLenum usage;
    if (dynamic)
    {
        usage = GL_DYNAMIC_DRAW;
    }
    else
    {
        usage = GL_STATIC_DRAW;
    }

    glBufferData(GL_ARRAY_BUFFER, bufferSizeInBytes_, nullptr, usage);

    if (initialData != nullptr)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeInBytes_, initialData);
    }

    return true;
}

void* VertexBufferOpenGL::Map(MapFlag_t mapFlag) const
{
    return InternalMap(GetOGLMapFlag(mapFlag), GL_ARRAY_BUFFER);
}

void VertexBufferOpenGL::Unmap() const
{
    InternalUnmap(GL_ARRAY_BUFFER);
}

bool IndexBufferOpenGL::Initialize(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices)
{
    if (buffer_ > 0)
    {
        Logger::GetInstance()->Warning("Trying to initialize an already initialized index buffer");
        return false;
    }

    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);

    indexFormat_ = indexFormat;
    numIndices_ = numIndices;

    bufferSizeInBytes_ = numIndices_ * ( (indexFormat_ == IndexFormat_t::INT_2_BYTES) ? 2 : 4 );

    GLenum usage;
    if (dynamic)
    {
        usage = GL_DYNAMIC_DRAW;
    }
    else
    {
        usage = GL_STATIC_DRAW;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSizeInBytes_, nullptr, usage);

    if (initialData != nullptr)
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSizeInBytes_, initialData);
    }

    return true;
}

void* IndexBufferOpenGL::Map(MapFlag_t mapFlag) const
{
    return InternalMap(GetOGLMapFlag(mapFlag), GL_ELEMENT_ARRAY_BUFFER);
}

void IndexBufferOpenGL::Unmap() const
{
    InternalUnmap(GL_ELEMENT_ARRAY_BUFFER);
}

bool ConstantBufferOpenGL::Initialize(void* initialData, bool dynamic, bool immutable, size_t dataSize)
{
    if (buffer_ > 0)
    {
        Logger::GetInstance()->Warning("Trying to initialize an already initialized constant buffer");
        return false;
    }

    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer_);

    bufferSizeInBytes_ = dataSize;

    GLenum usage;
    if (dynamic)
    {
        usage = GL_DYNAMIC_DRAW;
    }
    else
    {
        usage = GL_STATIC_DRAW;
    }

    glBufferData(GL_UNIFORM_BUFFER, bufferSizeInBytes_, nullptr, usage);

    if (initialData != nullptr)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSizeInBytes_, initialData);
    }

    return true;
}

void* ConstantBufferOpenGL::Map(MapFlag_t mapFlag) const
{
    return InternalMap(GetOGLMapFlag(mapFlag), GL_UNIFORM_BUFFER);
}

void ConstantBufferOpenGL::Unmap() const
{
    InternalUnmap(GL_UNIFORM_BUFFER);
}

GLenum GetOGLMapFlag(MapFlag_t mapFlag)
{
    GLenum mapType;
    switch (mapFlag)
    {
    case MapFlag_t::READ:               mapType = GL_MAP_READ_BIT | GL_MAP_FLUSH_EXPLICIT_BIT; break;
    case MapFlag_t::WRITE:              mapType = GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT; break;
    case MapFlag_t::READ_WRITE:         mapType = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT; break;
    case MapFlag_t::WRITE_DISCARD:      mapType = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT; break;
    case MapFlag_t::WRITE_NO_OVERWRITE: mapType = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT; break;
    }

    return mapType;
}

}