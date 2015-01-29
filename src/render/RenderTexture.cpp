#include "render/RenderTexture.h"

namespace Sketch3D {


RenderTexture::RenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) : width_(width), height_(height), format_(format),
        depthBufferBound_(false), texturesAttached_(false)
{
}

RenderTexture::~RenderTexture() {
}

}