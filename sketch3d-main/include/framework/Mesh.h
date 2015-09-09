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

        const vector<shared_ptr<SubMesh>>&  GetSubMeshes() const;
        shared_ptr<SubMesh>                 GetSubMesh(size_t index) const;

    private:
        vector<shared_ptr<SubMesh>>         subMeshes_;
};

bool LoadMeshFromFile(const string& filename, HardwareResourceCreator* hardwareResourceCreator, shared_ptr<Mesh>& loadedMesh, bool calculateTangents=false);

}

#endif