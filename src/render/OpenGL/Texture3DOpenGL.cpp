#include "render/OpenGL/Texture3DOpenGL.h"

namespace Sketch3D {
Texture3DOpenGL::Texture3DOpenGL() : Texture3D(), textureName_(0) {
}

Texture3DOpenGL::Texture3DOpenGL(unsigned int width, unsigned int height, unsigned int depth, FilterMode_t filterMode,
                                 WrapMode_t wrapMode, TextureFormat_t format) :
        Texture3D(width, height, depth, filterMode, wrapMode, format), textureName_(0)
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

    glBindTexture(GL_TEXTURE_3D, textureName_);

    GLuint filter, wrap, format, components, type, bpp;
	switch (filterMode_) {
		case FILTER_MODE_NEAREST:
			filter = GL_NEAREST;
			break;

		case FILTER_MODE_LINEAR:
			filter = GL_LINEAR;
			break;
	}

	switch (wrapMode_) {
		case WRAP_MODE_CLAMP:
			wrap = GL_CLAMP;
			break;

		case WRAP_MODE_REPEAT:
			wrap = GL_REPEAT;
			break;

        case WRAP_MODE_CLAMP_TO_BORDER:
            wrap = GL_CLAMP_TO_BORDER;
            break;
	}

    GetOpenglTextureFormat(format_, format, components, type, bpp);

    glTexImage3D(GL_TEXTURE_3D, 0, format, width_, height_, depth_, 0, components, type, data_);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);

    glBindTexture(GL_TEXTURE_3D, 0);

    return true;
}

void Texture3DOpenGL::Bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureName_);
}

void Texture3DOpenGL::SetPixelDataBytesImp(unsigned char* data) {
    if (textureName_ != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, textureName_);

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width_, height_, depth_, components, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
}

void Texture3DOpenGL::SetPixelDataFloatsImp(float* data) {
    if (textureName_ != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, textureName_);

        GLuint format, components, type, bpp;
        GetOpenglTextureFormat(format_, format, components, type, bpp);

        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width_, height_, depth_, components, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_3D, 0);
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

}