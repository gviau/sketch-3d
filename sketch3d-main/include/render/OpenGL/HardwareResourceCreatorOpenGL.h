#ifndef SKETCH_3D_HARDWARE_RESOURCE_CREATOR_OPENGL_H
#define SKETCH_3D_HARDWARE_RESOURCE_CREATOR_OPENGL_H

#include "render/HardwareResourceCreator.h"

namespace Sketch3D {
/**
 * @class HardwareResourceCreatorOpenGL
 * OpenGL implementation of the hardware resource creator
 */
class SKETCH_3D_API HardwareResourceCreatorOpenGL : public HardwareResourceCreator {
    public:
        virtual shared_ptr<ConstantBuffer>      CreateConstantBuffer() override;
        virtual shared_ptr<DepthStencilTarget>  CreateDepthStencilTarget() override;
        virtual shared_ptr<IndexBuffer>         CreateIndexBuffer() override;
        virtual shared_ptr<FragmentShader>      CreateFragmentShader() override;
        virtual shared_ptr<RenderTarget>        CreateRenderTarget() override;
        virtual shared_ptr<SamplerState>        CreateSamplerState() override;
        virtual shared_ptr<Texture2D>           CreateTexture2D() override;
        virtual shared_ptr<VertexBuffer>        CreateVertexBuffer() override;
        virtual shared_ptr<VertexShader>        CreateVertexShader() override;
};
}
#endif