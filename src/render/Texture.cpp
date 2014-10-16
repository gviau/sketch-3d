#include "render/Texture.h"

namespace Sketch3D {

Texture::Texture() : width_(0), height_(0), filterMode_(FILTER_MODE_NEAREST),
					 wrapMode_(WRAP_MODE_CLAMP), format_(TEXTURE_FORMAT_RGB24)
{
}

Texture::Texture(unsigned int width, unsigned int height,
                 FilterMode_t filterMode, WrapMode_t wrapMode, TextureFormat_t format) : width_(width),
																                         height_(height),
																                         filterMode_(filterMode),
																                         wrapMode_(wrapMode),
                                                                                         format_(format)
{
}

Texture::~Texture() {
}

void Texture::SetWidth(unsigned int width) {
	width_ = width;
}

void Texture::SetHeight(unsigned int height) {
	height_ = height;
}

void Texture::SetFilterMode(FilterMode_t mode) {
	filterMode_ = mode;
}

void Texture::SetWrapMode(WrapMode_t mode) {
	wrapMode_ = mode;
}

void Texture::SetTextureFormat(TextureFormat_t format) {
    format_ = format;
}

unsigned int Texture::GetWidth() const {
	return width_;
}

unsigned int Texture::GetHeight() const {
	return height_;
}

FilterMode_t Texture::GetFilterMode() const {
	return filterMode_;
}

WrapMode_t Texture::GetWrapMode() const {
	return wrapMode_;
}

TextureFormat_t Texture::GetTextureFormat() const {
    return format_;
}

}