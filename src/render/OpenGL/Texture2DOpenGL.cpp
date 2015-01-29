#include "render/OpenGL/Texture2DOpenGL.h"

namespace Sketch3D {
Texture2DOpenGL::Texture2DOpenGL() : Texture2D(), textureName_(0) {
}

Texture2DOpenGL::Texture2DOpenGL(unsigned int width, unsigned int height, FilterMode_t filterMode,
                                 WrapMode_t wrapMode, TextureFormat_t format) : Texture2D(width, height, filterMode, wrapMode, format), textureName_(0)
{
}

Texture2DOpenGL::~Texture2DOpenGL() {
    if (textureName_ != 0) {
        glDeleteTextures(1, &textureName_);
    }
}

bool Texture2DOpenGL::Create(bool generateMipmaps) {
    if (textureName_ == 0) {
	    glGenTextures(1, &textureName_);
    }

    size_t textureUnit = Bind();

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

    if (data_ != nullptr) {
	    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0,
				     components, type, data_);

        if (generateMipmaps) {
            glGenerateMipmap(textureUnit);
        }

    } else if (format == GL_DEPTH_COMPONENT16) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, components, type, nullptr);
    } else {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, width_, height_);
    }

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

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
            internalFormat = GL_DEPTH_COMPONENT32;
            format = GL_DEPTH_COMPONENT;
            type = GL_UNSIGNED_INT;

            // Not needed
            bpp = 0;
            break;
	}
}

}