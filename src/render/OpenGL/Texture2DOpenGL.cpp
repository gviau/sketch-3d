#include "render/OpenGL/Texture2DOpenGL.h"

namespace Sketch3D {
Texture2DOpenGL::Texture2DOpenGL() : Texture2D(), textureName_(0) {
}

Texture2DOpenGL::Texture2DOpenGL(unsigned int width, unsigned int height, FilterMode_t filterMode,
                                 WrapMode_t wrapMode, TextureFormat_t format) : Texture2D(width, height, filterMode, wrapMode, format)
{
}

Texture2DOpenGL::~Texture2DOpenGL() {
}

bool Texture2DOpenGL::Create() {
	glGenTextures(1, &textureName_);
	glBindTexture(GL_TEXTURE_2D, textureName_);

	int filter, wrap, format;
	switch (filterMode_) {
		case FILTER_MODE_POINT:
			filter = GL_NEAREST;
			break;

		case FILTER_MODE_BILINEAR:
			filter = GL_LINEAR;
			break;

			// TODO
		case FILTER_MODE_TRILINEAR:
		default:
			filter = GL_LINEAR;
	}

	switch (wrapMode_) {
		case WRAP_MODE_CLAMP:
			wrap = GL_CLAMP;
			break;

		case WRAP_MODE_REPEAT:
			wrap = GL_REPEAT;
			break;
	}

    // FreeImage loads pixels in reverse
	switch (format_) {
		case TEXTURE_FORMAT_RGB24:
            format = GL_BGR;
			break;

		case TEXTURE_FORMAT_RGBA32:
			format = GL_BGRA;
			break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, (format == GL_BGR) ? 3 : 4, width_, height_, 0,
				 format, GL_UNSIGNED_BYTE, data_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture2DOpenGL::Bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureName_);
}

}