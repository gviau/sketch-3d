#ifndef SKETCH_3D_SKINNED_MESH_H
#define SKETCH_3D_SKINNED_MESH_H

#include "framework/Mesh.h"

#include "math/Matrix4x4.h"

#include "system/Platform.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D
{

class AnimationState;
class Skeleton;

struct Bone_t;

/**
 * @class SkinnedMesh
 * This class implements the required functionality to read and play animation from
 * a rigged character.
 */
class SKETCH_3D_API SkinnedMesh : public Mesh
{
public:

                                SkinnedMesh();
								SkinnedMesh(const Mesh& mesh);

    /**
     * Animate the skinned mesh.
     * @param deltaTime The time in seconds elapsed since the last frame
     * @return true If the transformation matrices were correctly calculated, false otherwise
     */
    bool                        Animate(double deltaTime);

    /**
     * Set the current animation state by its name
     * @param name The name of the animation state to set
     */
    void                        SetAnimationState(const string& name);

    /**
     * Determines weither or not the animation should loop.
     * @param looping If true, the animation will loop, otherwise, it will stop after it reached the end
     */
    void                        SetAnimationLoop(bool looping);

	/**
	 * Starts animating the mesh
	 */
	void						StartAnimation();

    /**
     * Stops animating the mesh
     */
    void                        StopAnimation();

    /**
     * Checks weither or not there is an animation currently running
     */
    bool                        IsAnimationRunning() const;

    const shared_ptr<Skeleton>& GetSkeleton() const { return m_Skeleton; }
	void						SetSkeleton(const shared_ptr<Skeleton>& skeleton, const map<string, size_t>& bonesNameToIndex);

    const AnimationState*       GetCurrentAnimationState() const { return m_CurrentAnimationState; }

	const vector<Matrix4x4>&	GetTransformationMatrices() const { return m_TransformationMatrices; }

private:
    shared_ptr<Skeleton>        m_Skeleton;  /**< The rigged skeleton of this mesh */
    double                      m_Time;      /**< Current animation time for this skinned mesh */
    const AnimationState*       m_CurrentAnimationState; /**< The animation state to use while animating the skeleton */
	bool						m_IsAnimating;			/**< Is the animation playing? */
    bool                        m_IsLooping;             /**< Is the animation looping? */

	map<const Bone_t*, size_t>  m_BoneToIndex;   /**< Map the bones to the index given to the vertices for skinning */

    vector<Matrix4x4>           m_TransformationMatrices; /**< The cached transformation matrices to animate the bones */
};

bool LoadSkinnedMeshFromFile(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<SkinnedMesh>& loadedMesh, bool calculateTangents=false);
bool LoadSkinnedMeshFromFileWithMaterial(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<SkinnedMesh>& loadedMesh,
                                         MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents=false);

}

#endif