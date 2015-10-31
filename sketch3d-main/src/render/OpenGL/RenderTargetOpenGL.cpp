#include "render/OpenGL/RenderTargetOpenGL.h"

#include "render/OpenGL/ErrorCheckingOpenGL.h"
#include "render/OpenGL/TextureOpenGL.h"

namespace Sketch3D {
RenderViewOpenGL::RenderViewOpenGL()
    : m_TextureName(0)
{
}

RenderViewOpenGL::~RenderViewOpenGL()
{
    if (m_TextureName > 0)
    {
        GL_CALL( glDeleteTextures(1, &m_TextureName) );
    }
}

size_t RenderViewOpenGL::GetTextureName() const
{
    return m_TextureName;
}

bool RenderTargetOpenGL::Initialize(size_t width, size_t height, TextureFormat_t textureFormat)
{
    if (m_TextureName > 0)
    {
        return false;
    }

    width_ = width;
    height_ = height;
    format_ = textureFormat;

    GL_CALL( glGenTextures(1, &m_TextureName) );

    GL_CALL( glBindTexture(GL_TEXTURE_2D, m_TextureName) );

    TextureFormat_t format = format_;
    if (format == TextureFormat_t::RGB24)
    {
        format = TextureFormat_t::RGBA32;
    }

    GLuint internalFormat, components, type, bytesPerPixel;
    GetOGLTextureFormat(format, internalFormat, components, type, bytesPerPixel);

    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );
    GL_CALL( glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, components, type, nullptr) );

    return true;
}

bool DepthStencilTargetOpenGL::Initialize(size_t width, size_t height, DepthStencilBits_t depthStencilBits)
{
    if (m_TextureName > 0)
    {
        return false;
    }

    width_ = width;
    height_ = height;

    GL_CALL( glGenTextures(1, &m_TextureName) );

    GL_CALL( glBindTexture(GL_TEXTURE_2D, m_TextureName) );

    GLuint internalFormat, components, type, bytesPerPixel;
    GetOGLTextureFormat(TextureFormat_t::DEPTH, internalFormat, components, type, bytesPerPixel);

    GL_CALL( glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, components, type, nullptr) );

    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE) );

    return true;
}

}