#ifndef SKETCH_3D_SKELETON_H
#define SKETCH_3D_SKELETON_H

#include "math/Matrix4x4.h"

#include "render/AnimationState.h"

#include "system/Common.h"

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @struct Bone_t
 * This struct represents a bone in a skeleton for a skinned mesh
 */
struct Bone_t {
    Bone_t() {}
    Bone_t(const Matrix4x4& offset) : offsetMatrix(offset) {}
    vector<Bone_t*> linkedBones;
    Matrix4x4 offsetMatrix;
    string name;
    unordered_map<size_t, float> vertexWeight;
};

/**
 * @class Skeleton
 * This class represents a skeleton that should be used with a SkinnedMesh. It contains
 * the information regarding the bones, the vertices that are attached to them and the animations
 * that the skeleton posses.
 */
class Skeleton {
    typedef unordered_map<string, Bone_t> BoneCacheMap_t;

    public:
        /**
         * Constructor
         */
                                    Skeleton();

        /**
         * Destructor
         */
                                   ~Skeleton();

        /**
         * This function returns the updated transform matrix for each bones in the skeleton
         * @param deltaTime The current time in seconds
         * @param animationState The animation state to use to animate the skeleton
         * @param isLooping Should the animation loop over
         * @param transformationMatrices A map where the key is the bone and the key is
         * the updated transformation matrix to use to transform the appropriate vertices
         * @return true if the skeleton could calculate the transformation matrices, false if there
         * are no animation state chosen or if the current animation state is not looping and finished
         */
        bool                        GetTransformationMatrices(double time, const AnimationState* animationState, bool isLooping,
                                                              map<const Bone_t*, Matrix4x4>& transformationMatrices) const;

        /**
         * Add an animation state to the skeleton
         * @param name The name of the animation
         * @param animationState The animation state to add
         */
        void                        AddAnimationState(const string& name, const AnimationState& animationState);

        /**
         * Get an animation state by its name from the skeleton
         * @param name The name of the animation
         * @return A pointer to the animation state if it exists for this skeleton, nullptr otherwise
         */
        const AnimationState*       GetAnimationState(const string& name) const;

        /**
         * Create a bone. This bone will not be attached to any other. If
         * this is the first bone created, then it will become the root.
         * @param name The name of the bone to retrieve
         * @param offsetMatrix The matrix that transforms the bone from mesh space to its bind pose
         */
        Bone_t*                     CreateBone(const string& name, const Matrix4x4& offsetMatrix);

        /**
         * Retrieve a bone by its name
         * @param name The name of the bone to retrieve
         */
        Bone_t*                     FindBoneByName(const string& name);

        /**
         * Set the global inverse transform of the skeleton
         * @param globalInverseTransform The matrix representing the global inverse transform of the skeleton
         */
        void                        SetGlobalInverseTransform(const Matrix4x4& globalInverseTransform);

        INLINE size_t               GetNumberOfBones() const { return bones_.size(); }

    private:
        map<string, AnimationState> animationStates_;   /**< The animation that this skeleton can perform */
        BoneCacheMap_t              bones_; /**< List of bones of the skeleton */
        Bone_t*                     root_;  /**< Root bone of the skeleton */
        Matrix4x4                   globalInverseTransform_;   /**< The global inverse transform of the skeleton */

        /**
         * Calculate the final bone transformation matrix to use according to the time and the
         * bones' parent
         * @param time The current time in seconds of the animation
         * @param animationState The animation state to use
         * @param bone The current bone for which to calculate the final transformation matrix
         * @param parentTransformation The parent transformation matrix
         * @param transformationMatrices A map where the key is the bone and the key is
         * the updated transformation matrix to use to transform the appropriate vertices
         */
        void                        CalculateBoneTransformation(double time, const AnimationState* animationState,
                                                                const Bone_t* bone, const Matrix4x4& parentTransformation,
                                                                map<const Bone_t*, Matrix4x4>& transformationMatrices) const;

        /**
         * Calculate the interpolated scaling vector according to the current time
         * @param time The current time in seconds of the animation
         * @param animationState The animation state to use
         * @param bone A pointer to the bone for which we want to calculate the interpolated
         * value.
         * @return The interpolated scaling vector
         */
        Vector3                     CalculateInterpolatedScaling(double time, const AnimationState* animationState,
                                                                 const Bone_t* bone) const;

        /**
         * Calculate the interpolated rotation quaternion according to the current time
         * @param time The current time in seconds of the animation
         * @param animationState The animation state to use
         * @param bone A pointer to the bone for which we want to calculate the interpolated
         * value.
         * @return The interpolated rotation quaternion
         */
        Quaternion                  CalculateInterpolatedRotation(double time, const AnimationState* animationState,
                                                                  const Bone_t* bone) const;

        /**
         * Calculate the interpolated translation vector according to the current time
         * @param time The current time in seconds of the animation
         * @param animationState The animation state to use
         * @param bone A pointer to the bone for which we want to calculate the interpolated
         * value.
         * @return The interpolated translation vector
         */
        Vector3                     CalculateInterpolatedTranslation(double time, const AnimationState* animationState,
                                                                     const Bone_t* bone) const;
};

}

#endif
