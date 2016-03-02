#include "framework/VisualNode.h"

namespace Sketch3D
{
void VisualNode::AddChild(VisualNode* childNode)
{
    m_Children.push_back(childNode);
}
}