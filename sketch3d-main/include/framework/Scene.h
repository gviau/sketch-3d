#ifndef SKETCH_3D_SCENE_H
#define SKETCH_3D_SCENE_H

#include "framework/VisualNodes/VisualNode.h"

#include <memory>
#include <vector>
using namespace std;

namespace Sketch3D
{

class Light;

/**
 * @class Scene
 * The scene class is a collection of VisualNode arranged as a tree and a list of lights
 */
class Scene
{
public:
										Scene();
									   ~Scene();

    void                                SetLights(const vector<shared_ptr<Light>>& lights) { m_Lights = lights; }

    VisualNode&                         GetRootNode() { return *m_RootNode; }
    const VisualNode&                   GetRootNode() const { return *m_RootNode; }

    const vector<shared_ptr<Light>>&    GetLights() const { return m_Lights; }

private:
    VisualNode*                         m_RootNode;
    vector<shared_ptr<Light>>           m_Lights;
};
}

#endif