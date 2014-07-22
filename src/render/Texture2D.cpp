#include "render/Texture2D.h"

#include "render/ResourceManager.h"

namespace Sketch3D {

Texture2D::Texture2D() : Texture() {
}

Texture2D::Texture2D(unsigned int width, unsigned int height,
					 FilterMode_t filterMode, WrapMode_t wrapMode,
					 TextureFormat_t format) : Texture(width, height, filterMode, wrapMode),
											   format_(format)
{
}

Texture2D::~Texture2D() {
}

void Texture2D::SetTextureFormat(TextureFormat_t format) {
	format_ = format;
}

TextureFormat_t Texture2D::GetTextureFormat() const {
	return format_;
}

}