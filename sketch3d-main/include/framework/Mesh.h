#ifndef SKETCH_3D_MESH_H
#define SKETCH_3D_MESH_H

#include "render/VertexFormat.h"

#include <memory>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward class declarations
class HardwareResourceCreator;
class IndexBuffer;
class Material;
class MaterialCodeGenerator;
class RenderDevice;
class SubMesh;
class VertexBuffer;

/**
 * @class Mesh
 * The mesh class is composed of sub meshes. It acts as a container for them
 */
class SKETCH_3D_API Mesh
{
    public:
        void                                Draw(const shared_ptr<RenderDevice>& renderDevice) const;

        void                                AddSubMesh(const shared_ptr<SubMesh>& subMesh);
        void                                RemoveSubMesh(size_t index);
        void                                ClearSubMeshes();

        void                                SetMaterialForAllSubMeshes(const shared_ptr<Material>& material);

        const vector<shared_ptr<SubMesh>>&  GetSubMeshes() const;
        shared_ptr<SubMesh>                 GetSubMesh(size_t index) const;

    private:
        vector<shared_ptr<SubMesh>>         m_SubMeshes;
};

bool LoadMeshFromFile(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh, bool calculateTangents=false);
bool LoadMeshFromFileWithMaterial(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh,
                                  MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents=false);

}

#endif