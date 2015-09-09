#ifndef SKETCH_3D_SUB_MESH_H
#define SKETCH_3D_SUB_MESH_H

#include "system/Platform.h"

#include <memory>
using namespace std;

namespace Sketch3D {

// Forward class declaration
class IndexBuffer;
class Material;
class RenderDevice;
class VertexBuffer;

/**
 * @class SubMesh The sub mesh class is the smallest drawing unit.
 */
class SKETCH_3D_API SubMesh
{
    public:
        void                            Draw(const shared_ptr<RenderDevice>& renderDevice) const;

        void                            SetVertexBuffer(const shared_ptr<VertexBuffer>& vertexBuffer) { vertexBuffer_ = vertexBuffer; }
        void                            SetIndexBuffer(const shared_ptr<IndexBuffer>& indexBuffer) { indexBuffer_ = indexBuffer; }
        void                            SetMaterial(const shared_ptr<Material>& material) { material_ = material; }

        const shared_ptr<VertexBuffer>& GetVertexBuffer() const { return vertexBuffer_; }
        const shared_ptr<IndexBuffer>&  GetIndexBuffer() const { return indexBuffer_; }
        const shared_ptr<Material>&     GetMaterial() const { return material_; }

    private:
        shared_ptr<VertexBuffer>        vertexBuffer_;
        shared_ptr<IndexBuffer>         indexBuffer_;
        shared_ptr<Material>            material_;
};

}

#endif