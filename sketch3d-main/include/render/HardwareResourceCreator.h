#ifndef SKETCH_3D_HARDWARE_RESOURCE_CREATOR_H
#define SKETCH_3D_HARDWARE_RESOURCE_CREATOR_H

#include "system/Platform.h"

#include "render/RenderDevice_Common.h"
#include "render/Renderer_Common.h"
#include "render/SamplerState.h"
#include "render/Texture.h"
#include "render/VertexFormat.h"

#include <memory>
using namespace std;

namespace Sketch3D {
// Forward class declaration
class ConstantBuffer;
class DepthStencilTarget;
class IndexBuffer;
class FragmentShader;
class RenderTarget;
class VertexBuffer;
class VertexShader;

/**
 * @class HardwareResourceCreator
 * Base class for API dependent hardware resource creation
 */
class SKETCH_3D_API HardwareResourceCreator {
    public:
        virtual shared_ptr<ConstantBuffer>      CreateConstantBuffer() = 0;
        virtual shared_ptr<DepthStencilTarget>  CreateDepthStencilTarget() = 0;
        virtual shared_ptr<IndexBuffer>         CreateIndexBuffer() = 0;
        virtual shared_ptr<FragmentShader>      CreateFragmentShader() = 0;
        virtual shared_ptr<RenderTarget>        CreateRenderTarget() = 0;
        virtual shared_ptr<SamplerState>        CreateSamplerState() = 0;
        virtual shared_ptr<Texture2D>           CreateTexture2D() = 0;
        virtual shared_ptr<VertexBuffer>        CreateVertexBuffer() = 0;
        virtual shared_ptr<VertexShader>        CreateVertexShader() = 0;
};
}

#endif