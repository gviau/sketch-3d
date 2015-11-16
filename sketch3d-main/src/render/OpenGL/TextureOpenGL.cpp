#include "render/OpenGL/TextureOpenGL.h"

#include "render/TextureMap.h"

#include "render/OpenGL/ErrorCheckingOpenGL.h"
#include "render/OpenGL/SamplerStateOpenGL.h"

#include "system/Logger.h"

namespace Sketch3D {

TextureOpenGL::TextureOpenGL()
    : texture_(0)
{
}

TextureOpenGL::~TextureOpenGL()
{
    if (texture_ > 0)
    {
        GL_CALL( glDeleteTextures(1, &texture_) );
    }
}

GLuint TextureOpenGL::GetTexture() const
{
    return texture_;
}

bool Texture1DOpenGL::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable)
{
    return false;
}

void Texture1DOpenGL::ApplySamplerState(SamplerState* samplerState)
{
}

void Texture1DOpenGL::Bind()
{
}

bool Texture2DOpenGL::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable)
{
    if (texture_ == 0)
    {
        GL_CALL( glGenTextures(1, &texture_) );
    }

    textureMap_ = textureMap;
    textureFormat_ = textureFormat;

    GL_CALL( glBindTexture(GL_TEXTURE_2D, texture_) );

    TextureFormat_t format = textureFormat_;
    if (format == TextureFormat_t::RGB24)
    {
        format = TextureFormat_t::RGBA32;
    }

    GLuint internalFormat, components, type, bytesPerPixel;
    GetOGLTextureFormat(format, internalFormat, components, type, bytesPerPixel);

    GL_CALL( glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureMap->GetWidth(), textureMap->GetHeight(), 0, components, type, textureMap->GetData()) );

    if (format == TextureFormat_t::DEPTH)
    {
        GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE) );
        GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL) );
        GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE) );
    }

    return true;
}

void Texture2DOpenGL::ApplySamplerState(SamplerState* samplerState)
{
    if (samplerState == nullptr)
    {
        return;
    }

    FilterMode_t filterMode = samplerState->GetFilterMode();
    AddressMode_t addressModeU = samplerState->GetAddressModeU();
    AddressMode_t addressModeV = samplerState->GetAddressModeV();
    AddressMode_t addressModeW = samplerState->GetAddressModeW();
    ComparisonFunction_t comparisonFunction = samplerState->GetComparisonFunction();

    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetOGLFilterMode(filterMode)) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetOGLFilterMode(filterMode)) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetOGLAddressMode(addressModeU)) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetOGLAddressMode(addressModeV)) );
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GetOGLComparisonFunction(comparisonFunction)) );
}

void Texture2DOpenGL::Bind()
{
    if (texture_ > 0)
    {
        GL_CALL( glBindTexture(GL_TEXTURE_2D, texture_) );
    }
}

bool Texture3DOpenGL::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable)
{
    return false;
}

void Texture3DOpenGL::ApplySamplerState(SamplerState* samplerState)
{
}

void Texture3DOpenGL::Bind()
{
}

void GetOGLTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& components, GLuint& type, GLuint& bytesPerPixel)
{
	switch (textureFormat)
    {
    case TextureFormat_t::GRAYSCALE:
        internalFormat = GL_R8;
        components = GL_RED;
        type = GL_UNSIGNED_BYTE;
        bytesPerPixel = 1;
        break;

	case TextureFormat_t::RGB24:
        internalFormat = GL_RGB;
        components = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        bytesPerPixel = 3;
		break;

	case TextureFormat_t::RGBA32:
		internalFormat = GL_RGBA;
        components = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        bytesPerPixel = 4;
		break;

    case TextureFormat_t::R32F:
        internalFormat = GL_R32F;
        components = GL_RED;
        type = GL_FLOAT;
        bytesPerPixel = 1;
        break;

    case TextureFormat_t::RG32F:
        internalFormat = GL_RG32F;
        components = GL_RG;
        type = GL_FLOAT;
        bytesPerPixel = 2;
        break;

    case TextureFormat_t::RGBA32F:
        internalFormat = GL_RGBA32F;
        components = GL_RGBA;
        type = GL_FLOAT;
        bytesPerPixel = 4;
        break;

    case TextureFormat_t::R16F:
        internalFormat = GL_R16F;
        components = GL_R;
        type = GL_FLOAT;
        bytesPerPixel = 1;
        break;

    case TextureFormat_t::RG16F:
        internalFormat = GL_RG16F;
        components = GL_RG;
        type = GL_FLOAT;
        bytesPerPixel = 2;
        break;

    case TextureFormat_t::RGBA16F:
        internalFormat = GL_RGBA16F;
        components = GL_RGBA;
        type = GL_FLOAT;
        bytesPerPixel = 4;
        break;

    case TextureFormat_t::DEPTH:
        internalFormat = GL_DEPTH_COMPONENT32;
        components = GL_DEPTH_COMPONENT;
        type = GL_UNSIGNED_INT;

        // Not needed
        bytesPerPixel = 0;
        break;
	}
}

}