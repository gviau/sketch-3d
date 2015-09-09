#include "framework/SubMesh.h"

#include "framework/Material.h"

#include "render/Buffer.h"
#include "render/RenderDevice.h"

#include "system/Logger.h"

namespace Sketch3D {

void SubMesh::Draw(const shared_ptr<RenderDevice>& renderDevice) const
{
    VertexBuffer* vertexBuffer = vertexBuffer_.get();
    if (vertexBuffer == nullptr)
    {
        Logger::GetInstance()->Warning("Trying to a draw a submesh with a null vertex buffer");
        return;
    }

    if (material_ != nullptr)
    {
        material_->Apply();
    }

    IndexBuffer* indexBuffer = indexBuffer_.get();
    if (indexBuffer == nullptr)
    {
        renderDevice->Draw(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer_, 0);
    }
    else
    {
        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer_, indexBuffer_, 0, 0);
    }
}

}