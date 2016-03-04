#ifndef SKETCH_3D_SIMPLE_OBJECT_H
#define SKETCH_3D_SIMPLE_OBJECT_H

#include <memory>
using namespace std;

namespace Sketch3D
{

class MaterialCodeGenerator;
class Mesh;
class RenderDevice;

extern shared_ptr<Mesh> g_FullscreenQuad;

extern shared_ptr<Mesh> g_UnitCubeMesh;
extern shared_ptr<Mesh> g_UnitSphereMesh;

// If no rendering pipeline is provided, there will be no shaders in the material and they will have to be provided by the user
void InitializeSimpleObjects(const shared_ptr<RenderDevice>& renderDevice, MaterialCodeGenerator* materialCodeGenerator);
}

#endif