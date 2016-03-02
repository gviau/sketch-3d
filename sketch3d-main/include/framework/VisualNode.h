#ifndef SKETCH_3D_VISUAL_NODE_H
#define SKETCH_3D_VISUAL_NODE_H

#include <vector>
using namespace std;

namespace Sketch3D
{
/**
 * @class VisualNode
 * The VisualNode class is a wrapper around a Mesh object and allows the user to give it a position, an orientation
 * and a scale. It is also possible to attach children to a VisualNode.
 */
class VisualNode
{
public:

    void                        AddChild(VisualNode* childNode);

    const vector<VisualNode*>&  GetChildren() const { return m_Children; }

private:
    vector<VisualNode*>         m_Children;
};
}

#endif