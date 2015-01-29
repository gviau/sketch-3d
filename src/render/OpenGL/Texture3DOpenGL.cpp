#include "render/OpenGL/Texture3DOpenGL.h"

namespace Sketch3D {
Texture3DOpenGL::Texture3DOpenGL(bool generateMipmaps) : Texture3D(generateMipmaps), textureName_(0) {
}

Texture3DOpenGL::Texture3DOpenGL(unsigned int width, unsigned int height, unsigned int depth, bool generateMipmaps,
                                 FilterMode_t filterMode, WrapMode_t wrapMode, TextureFormat_t format) :
        Texture3D(width, height, depth, generateMipmaps, filterMode, wrapMode, format), textureName_(0)
{
}

Texture3DOpenGL::~Texture3DOpenGL() {
    if (textureName_ != 0) {
        glDeleteTextures(1, &textureName_);
    }
}

bool Texture3DOpenGL::Create() {
    if (textureName_ == 0) {
        glGenTextures(1, &textureName_);
    }

    Bind();

    GLuint filter = GetOpenglFilterMode();
    GLuint wrap = GetOpenglWrapMode();
    
    GLuint format, components, type, bpp;
    GetOpenglTextureFormat(format_, format, components, type, bpp);

    glTexImage3D(GL_TEXTURE_3D, 0, format, width_, height_, depth_, 0, components, type, data_);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);

    return true;
}

void Texture3DOpenGL::SetFilterModeImpl() const {
    Bind();

    GLuint filter = GetOpenglFilterMode();

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture3DOpenGL::SetWrapModeImpl() const {
    Bind();

    GLuint wrap = GetOpenglWrapMode();

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);
}

void Texture3DOpenGL::SetPixelDataBytesImp(unsigned char* data) {
    if (textureName_ != 0) {
        Bind();

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width_, height_, depth_, components, GL_UNSIGNED_BYTE, data);
    }
}

void Texture3DOpenGL::SetPixelDataFloatsImp(float* data) {
    if (textureName_ != 0) {
        Bind();

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width_, height_, depth_, components, GL_FLOAT, data);
    }
}

void Texture3DOpenGL::GetOpenglTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& format, GLuint& type, GLuint& bpp) const {
	switch (textureFormat) {
		case TEXTURE_FORMAT_RGB24:
            internalFormat = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            bpp = 3;
			break;

		case TEXTURE_FORMAT_RGBA32:
			internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            bpp = 4;
			break;

        // FreeImage loads pixels in reverse
        case TEXTURE_FORMAT_BGR24:
            internalFormat = 3;
            format = GL_BGR;
            type = GL_UNSIGNED_BYTE;
            bpp = 3;
            break;

        case TEXTURE_FORMAT_BGRA32:
            internalFormat = 4;
            format = GL_BGRA;
            type = GL_UNSIGNED_BYTE;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_R32F:
            internalFormat = GL_R32F;
            format = GL_RED;
            type = GL_FLOAT;
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RG32F:
            internalFormat = GL_RG32F;
            format = GL_RG;
            type = GL_FLOAT;
            bpp = 2;
            break;

        case TEXTURE_FORMAT_RGB32F:
            internalFormat = GL_RGB32F;
            format = GL_RGB;
            type = GL_FLOAT;
            bpp = 3;
            break;

        case TEXTURE_FORMAT_RGBA32F:
            internalFormat = GL_RGBA32F;
            format = GL_RGBA;
            type = GL_FLOAT;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_DEPTH:
            internalFormat = GL_DEPTH_COMPONENT16;
            format = GL_DEPTH_COMPONENT;
            type = GL_UNSIGNED_INT;

            // Not needed
            bpp = 0;
            break;
	}
}

GLuint Texture3DOpenGL::GetOpenglFilterMode() const {
	switch (filterMode_) {
		case FILTER_MODE_NEAREST:
			return GL_NEAREST;

		case FILTER_MODE_LINEAR:
			return GL_LINEAR;
	}

    return 0;
}

GLuint Texture3DOpenGL::GetOpenglWrapMode() const {
	switch (wrapMode_) {
		case WRAP_MODE_CLAMP:
			return GL_CLAMP;

		case WRAP_MODE_REPEAT:
			return GL_REPEAT;

        case WRAP_MODE_CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
	}

    return 0;
}

}