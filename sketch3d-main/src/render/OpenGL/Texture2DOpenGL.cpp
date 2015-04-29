#include "render/OpenGL/Texture2DOpenGL.h"

namespace Sketch3D {
Texture2DOpenGL::Texture2DOpenGL(bool generateMipmaps) : Texture2D(generateMipmaps), textureName_(0) {
}

Texture2DOpenGL::Texture2DOpenGL(unsigned int width, unsigned int height, bool generateMipmaps, FilterMode_t filterMode,
                                 WrapMode_t wrapMode, TextureFormat_t format) : Texture2D(width, height, generateMipmaps,
                                                                                          filterMode, wrapMode, format), textureName_(0)
{
}

Texture2DOpenGL::~Texture2DOpenGL() {
    if (textureName_ != 0) {
        glDeleteTextures(1, &textureName_);
    }
}

bool Texture2DOpenGL::Create() {
    if (textureName_ == 0) {
	    glGenTextures(1, &textureName_);
    }

    Bind();

    GLuint filter = GetOpenglFilterMode();
    GLuint wrap = GetOpenglWrapMode();

    GLuint format, components, type, bpp;
    GetOpenglTextureFormat(format_, format, components, type, bpp);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    if (format_ == TEXTURE_FORMAT_DEPTH) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, components, type, data_);

    if (generateMipmaps_) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return true;
}

const void* Texture2DOpenGL::GetData() const {
    if (data_ != nullptr) {
        return data_;
    }

    Bind();

    GLuint format, components, type, bpp;
    GetOpenglTextureFormat(format_, format, components, type, bpp);
    
    // TODO change that
    float* data = new float[width_ * height_ * bpp];
    glGetTexImage(GL_TEXTURE_2D, 0, components, type, data);

    return data;
}

void Texture2DOpenGL::SetFilterModeImpl() const {
    Bind();

    GLuint filter = GetOpenglFilterMode();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture2DOpenGL::SetWrapModeImpl() const {
    Bind();

    GLuint wrap = GetOpenglWrapMode();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture2DOpenGL::SetPixelDataBytesImp(unsigned char* data) {
    if (textureName_ != 0) {
        Bind();

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, components, GL_UNSIGNED_BYTE, data);
    }
}

void Texture2DOpenGL::SetPixelDataFloatsImp(float* data) {
    if (textureName_ != 0) {
        Bind();

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, components, GL_FLOAT, data);
    }
}

void Texture2DOpenGL::GetOpenglTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& format, GLuint& type, GLuint& bpp) const {
	switch (textureFormat) {
        case TEXTURE_FORMAT_GRAYSCALE:
            internalFormat = GL_R8;
            format = GL_RED;
            type = GL_UNSIGNED_BYTE;
            bpp = 1;
            break;

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

        case TEXTURE_FORMAT_RGBA32F:
            internalFormat = GL_RGBA32F;
            format = GL_RGBA;
            type = GL_FLOAT;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_R16F:
            internalFormat = GL_R16F;
            format = GL_R;
            type = GL_FLOAT;
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RG16F:
            internalFormat = GL_RG16F;
            format = GL_RG;
            type = GL_FLOAT;
            bpp = 2;
            break;

        case TEXTURE_FORMAT_RGBA16F:
            internalFormat = GL_RGBA16F;
            format = GL_RGBA;
            type = GL_FLOAT;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_DEPTH:
            internalFormat = GL_DEPTH_COMPONENT32;
            format = GL_DEPTH_COMPONENT;
            type = GL_UNSIGNED_INT;

            // Not needed
            bpp = 0;
            break;
	}
}

GLuint Texture2DOpenGL::GetOpenglFilterMode() const {
	switch (filterMode_) {
		case FILTER_MODE_NEAREST:
			return (generateMipmaps_) ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST;

		case FILTER_MODE_LINEAR:
			return (generateMipmaps_) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
	}

    return 0;
}

GLuint Texture2DOpenGL::GetOpenglWrapMode() const {
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