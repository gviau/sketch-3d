#include "render/OpenGL/RenderTargetOpenGL.h"

#include "render/TextureMap.h"

#include "render/OpenGL/ErrorCheckingOpenGL.h"
#include "render/OpenGL/TextureOpenGL.h"

namespace Sketch3D {
RenderViewOpenGL::RenderViewOpenGL()
    : m_InternalTexture(nullptr)
{
}

RenderViewOpenGL::~RenderViewOpenGL()
{
    if (m_InternalTexture != nullptr)
    {
        delete m_InternalTexture;
    }
}

Texture2DOpenGL* RenderViewOpenGL::GetTexture() const
{
    return m_InternalTexture;
}

bool RenderTargetOpenGL::Initialize(size_t width, size_t height, TextureFormat_t textureFormat)
{
    if (m_InternalTexture != nullptr)
    {
        return false;
    }

    m_InternalTexture = new Texture2DOpenGL;

    width_ = width;
    height_ = height;
    format_ = textureFormat;

    TextureFormat_t format = format_;
    if (format == TextureFormat_t::RGB24)
    {
        format = TextureFormat_t::RGBA32;
    }

    TextureMap textureMap(nullptr, width_, height_);
    m_InternalTexture->Initialize(&textureMap, format, false);

    return true;
}

bool DepthStencilTargetOpenGL::Initialize(size_t width, size_t height, DepthStencilBits_t depthStencilBits)
{
    if (m_InternalTexture != nullptr)
    {
        return false;
    }

    width_ = width;
    height_ = height;

    m_InternalTexture = new Texture2DOpenGL;

    TextureMap textureMap(nullptr, width_, height_);
    m_InternalTexture->Initialize(&textureMap, TextureFormat_t::DEPTH, false);

    return true;
}

}