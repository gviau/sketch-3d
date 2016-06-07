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

class ConstantBuffer;
class RenderDevice;
struct RenderingPipelineContext;

/**
 * @class VisualNode
 * The VisualNode class is a wrapper around a Mesh object and allows the user to give it a position, an orientation
 * and a scale. It is also possible to attach children to a VisualNode.
 */
class VisualNode
{
public:
                                            VisualNode();

	virtual void							FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime) = 0;
	virtual bool							IsNodeValidForRendering() const { return false; }
	virtual void							RenderNode(const shared_ptr<RenderDevice>& renderDevice,
													   const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
													   const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
													   size_t materialConstantsSlot) = 0;

    void                                    AddChild(const shared_ptr<VisualNode>& childNode);

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

    // Used to access the material constants for every submeshes
    void                                    SetAmbientColor(const Vector3& ambientColor) { m_AmbientColor = ambientColor; }
    void                                    SetDiffuseColor(const Vector3& diffuseColor) { m_DiffuseColor = diffuseColor; }
    void                                    SetSpecularColor(const Vector3& specularColor) { m_SpecularColor = specularColor; }
    void                                    SetSpecularPower(float power) { m_SpecularPower = power; }

    const Vector3&                          GetAmbientColor() const { return m_AmbientColor; }
    const Vector3&                          GetDiffuseColor() const { return m_DiffuseColor; }
    const Vector3&                          GetSpecularColor() const { return m_SpecularColor; }
    float                                   GetSpecularPower() const { return m_SpecularPower; }

    const Vector3&                          GetPosition() const { return m_Position; }
    const Quaternion&                       GetOrientation() const { return m_Orientation; }
    const Vector3&                          GetScale() const { return m_Scale; }

    const vector<shared_ptr<VisualNode>>&   GetChildren() const { return m_Children; }

    const Matrix4x4&                        GetModelMatrix();

protected:
    VisualNode*                             m_Parent;
    vector<shared_ptr<VisualNode>>          m_Children;

    Vector3                                 m_Position;
    Quaternion                              m_Orientation;
    Vector3                                 m_Scale;

    Vector3                                 m_AmbientColor;
    Vector3                                 m_DiffuseColor;
    Vector3                                 m_SpecularColor;
    float                                   m_SpecularPower;

    bool                                    m_NeedsTransformationMatrixUpdate;
    Matrix4x4                               m_ModelMatrix;

    void                                    NotifyParentForTransformationMatrixUpdate_r();
	void									FillRenderingPipelineContextCommon(RenderingPipelineContext& renderingPipelineContext);
};
}

#endif