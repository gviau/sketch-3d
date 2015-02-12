#ifndef SKETCH_3D_SKINNED_MESH_H
#define SKETCH_3D_SKINNED_MESH_H

#include "math/Matrix4x4.h"

#include "render/Mesh.h"
#include "render/Skeleton.h"

#include "system/Platform.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

// Forward declaration
namespace Assimp {
	class Importer;
}

namespace Sketch3D {
/**
 * @class SkinnedMesh
 * This class implements the required functionality to read and play animation from
 * a rigged character. For now, the skinned mesh supports a maximum of 4 weights per bone
 */
class SKETCH_3D_API SkinnedMesh : public Mesh {
    public:
        /**
         * Constructor
         * @param isSkinnedOnGpu Is the mesh to be skinned on the GPU. If it is false, then the vertices will be updated
         * and sent to the video card each frame, with the transformations done on the CPU. If set to true, then it
         * is the responsability of the class user to implement the transformations on the GPU via a shader.
         * @param counterClockWise Is the data loaded in counter clock wise order or clock wise?
         */
                                    SkinnedMesh(bool isSkinnedOnGpu=false, bool counterClockWise=true);

        /**
         * Constructor
         * @param filename The name of the file to load
         * @param vertexAttributes A map of the vertex attributes to use. Each entry is a pair<VertexAttributes_t, size_t> where the
         * key is the vertex attributes and the value is its attribute location.
         * @param isSkinnedOnGpu Is the mesh to be skinned on the GPU. If it is false, then the vertices will be updated
         * and sent to the video card each frame, with the transformations done on the CPU. If set to true, then it
         * is the responsability of the class user to implement the transformations on the GPU via a shader.
         * @param counterClockWise Is the data loaded in counter clock wise order or clock wise?
         */
                                    SkinnedMesh(const string& filename, const VertexAttributesMap_t& vertexAttributes,
                                                bool isSkinnedOnGpu=false, bool counterClockWise=true);

        /**
         * Destructor
         */
        virtual                    ~SkinnedMesh();

        /**
         * Load a mesh and reads the information regarding the skeleton and the animation data
         * @param filename The name of the file to load
         * @param vertexAttributes A map of the vertex attributes to use. Each entry is a pair<VertexAttributes_t, size_t> where the
         * key is the vertex attributes and the value is its attribute location.
         * @param counterClockWise Is the data loaded in counter clock wise order or clock wise?
         */
        virtual void                Load(const string& filename, const VertexAttributesMap_t& vertexAttributes, bool counterClockWise=true);

        /**
         * Initialize the mesh with geometry data - also construct the required data for the bones if is to be skinned on the gpu
         * @param vertexAttributes A map of the vertex attributes to use. Each entry is a pair<VertexAttributes_t, size_t> where the
         * key is the vertex attributes and the value is its attribute location.
         */
        virtual void                Initialize(const VertexAttributesMap_t& vertexAttributes);

        /**
         * Animate the skinned mesh. If this is a dynamic mesh, then the vertex in the buffer will
         * be updated in this function. If this is a static mesh, then this function will only provide
         * the caller with the transformation matrices to use in the shader
         * @param deltaTime The time in seconds elapsed since the last frame
         * @param boneTransformationMatrices A vector containing all the transformation matrices required by the shader
         * that will skin the mesh (if it is to be skinned on the GPU)
         * @return true If the transformation matrices were correctly calculated, false otherwise
         */
        bool                        Animate(double deltaTime, vector<Matrix4x4>& boneTransformationMatricesi=defaultParameter_);

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
         * Stops animating the mesh
         */
        void                        StopAnimation();

        /**
         * Checks weither or not there is an animation currently running
         */
        bool                        IsAnimationRunning() const;

    private:
        static vector<Matrix4x4>    defaultParameter_;

        Skeleton*                   skeleton_;  /**< The rigged skeleton of this mesh */
        double                      time_;      /**< Current animation time for this skinned mesh */
        const AnimationState*       currentAnimationState_; /**< The animation state to use while animating the skeleton */
        bool                        isLooping_;             /**< Is the animation looping? */
        map<const Bone_t*, size_t>  boneToIndex_;   /**< Map the bone to the index given to the vertex for GPU skinning */

        /**
         * Free the mesh memory
         */
        virtual void                    FreeMeshMemory();
        virtual void                    ConstructBoundingSphere();
};

}

#endif
