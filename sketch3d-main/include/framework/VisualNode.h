#ifndef SKETCH_3D_VISUAL_NODE_H
#define SKETCH_3D_VISUAL_NODE_H

#include "math/Matrix4x4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

#include <memory>
#include <vector>
using namespace std;

namespace Sketch3D
{

class Mesh;

/**
 * @class VisualNode
 * The VisualNode class is a wrapper around a Mesh object and allows the user to give it a position, an orientation
 * and a scale. It is also possible to attach children to a VisualNode.
 */
class VisualNode
{
public:
                                            VisualNode();

    void                                    AddChild(const shared_ptr<VisualNode>& childNode);

    void                                    SetMesh(const shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }

    void                                    SetPosition(const Vector3& position);
    void                                    SetPosition(float x, float y, float z);
    void                                    SetOrientation(const Quaternion& orientation);
    void                                    SetOrientation(float pitchInRadians, float yawInRadians, float rollInRadians);
    void                                    SetScale(const Vector3& scale);
    void                                    SetScale(float x, float y, float z);

    // Those methods accumulate the values
    void                                    Translate(const Vector3& translation);
    void                                    Translate(float x, float y, float z);
    void                                    RotateAroundAxis(const Vector3& axis, float angleInRadians);
    void                                    Pitch(float angleInRadians);
    void                                    Yaw(float angleInRadians);
    void                                    Roll(float angleInRadians);
    void                                    Scale(const Vector3& scaling);
    void                                    Scale(float x, float y, float z);

    const Vector3&                          GetPosition() const { return m_Position; }
    const Quaternion&                       GetOrientation() const { return m_Orientation; }
    const Vector3&                          GetScale() const { return m_Scale; }

    const shared_ptr<Mesh>&                 GetMesh() const { return m_Mesh; }
    const vector<shared_ptr<VisualNode>>&   GetChildren() const { return m_Children; }

    const Matrix4x4&                        GetModelMatrix();

private:
    shared_ptr<Mesh>                        m_Mesh;

    VisualNode*                             m_Parent;
    vector<shared_ptr<VisualNode>>          m_Children;

    Vector3                                 m_Position;
    Quaternion                              m_Orientation;
    Vector3                                 m_Scale;

    bool                                    m_NeedsTransformationMatrixUpdate;
    Matrix4x4                               m_ModelMatrix;

    void                                    NotifyParentForTransformationMatrixUpdate_r();
};
}

#endif