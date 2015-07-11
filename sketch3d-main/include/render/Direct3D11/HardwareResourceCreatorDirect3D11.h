#ifndef SKETCH_3D_HARDWARE_RESOURCE_CREATOR_DIRECT3D11_H
#define SKETCH_3D_HARDWARE_RESOURCE_CREATOR_DIRECT3D11_H

#include "render/HardwareResourceCreator.h"

// Forward struct declaration
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Sketch3D {
/**
 * @class HardwareResourceCreatorDirect3D11
 * Direct3D11 implementation of the hardware resource creator
 */
class SKETCH_3D_API HardwareResourceCreatorDirect3D11 : public HardwareResourceCreator {
    public:
                                            HardwareResourceCreatorDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);

        virtual shared_ptr<ConstantBuffer>  CreateConstantBuffer(void* initialData, bool dynamic, bool immutable, size_t dataSize) override;
        virtual shared_ptr<IndexBuffer>     CreateIndexBuffer(void* initialData, bool dynamic, bool immutable, IndexFormat_t indexFormat, size_t numIndices) override;
        virtual shared_ptr<FragmentShader>  CreateFragmentShader() override;
        virtual shared_ptr<VertexBuffer>    CreateVertexBuffer(void* initialData, bool dynamic, bool immutable, VertexFormat* vertexFormat, size_t numVertices) override;
        virtual shared_ptr<VertexShader>    CreateVertexShader() override;

    private:
        ID3D11Device*                       device_;
        ID3D11DeviceContext*                context_;
};
}

#endif