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

Texture2D::Texture2D(const string& filename, FilterMode_t filterMode,
					 WrapMode_t wrapMode) : Texture(0, 0, filterMode, wrapMode),
											format_(TEXTURE_FORMAT_RGB24)
{
	LoadTexture(filename);
}

bool Texture2D::LoadTexture(const string& filename) {
	return ResourceManager::GetInstance()->LoadTexture(filename, this);
}

void Texture2D::SetTextureFormat(TextureFormat_t format) {
	format_ = format;
}

TextureFormat_t Texture2D::GetTextureFormat() const {
	return format_;
}

}