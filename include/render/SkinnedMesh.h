#ifndef SKETCH_3D_SKINNED_MESH_H
#define SKETCH_3D_SKINNED_MESH_H

#include "render/Mesh.h"
#include "render/Skeleton.h"

#include <map>
#include <string>
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
class SkinnedMesh : public Mesh {
    public:
        /**
         * Constructor
         * @param meshType The type of the mesh. Dynamic means that the update of the vertices according
         * of the bones' transforms will be done on the CPU. Static means that it is the responsability
         * of the class user to implement it on the GPU.
         */
                                    SkinnedMesh(MeshType_t meshType=MESH_TYPE_DYNAMIC);

        /**
         * Constructor
         * @param filename The name of the file to load
         * @param vertexAttributes A map of the vertex attributes to use. Each entry is a pair<VertexAttributes_t, size_t> where the
         * key is the vertex attributes and the value is its attribute location.
         * @param meshType The type of the mesh. Dynamic means that the update of the vertices according
         * of the bones' transforms will be done on the CPU. Static means that it is the responsability
         * of the class user to implement it on the GPU.
         */
                                    SkinnedMesh(const string& filename, const VertexAttributesMap_t& vertexAttributes,
                                                MeshType_t meshType=MESH_TYPE_DYNAMIC);

        /**
         * Destructor
         */
        virtual                    ~SkinnedMesh();

        /**
         * Load a mesh and reads the information regarding the skeleton and the animation data
         * @param filename The name of the file to load
         * @param vertexAttributes A map of the vertex attributes to use. Each entry is a pair<VertexAttributes_t, size_t> where the
         * key is the vertex attributes and the value is its attribute location.
         */
        virtual void                Load(const string& filename, const VertexAttributesMap_t& vertexAttributes);

        /**
         * Animate the skinned mesh. If this is a dynamic mesh, then the vertex in the buffer will
         * be updated in this function. If this is a static mesh, then this function will only provide
         * the caller with the transformation matrices to use in the shader
         * @param deltaTime The time in seconds elapsed since the last frame
         */
        void                        Animate(double deltaTime);

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
        Skeleton*                   skeleton_;  /**< The rigged skeleton of this mesh */
        double                      time_;      /**< Current animation time for this skinned mesh */
        const AnimationState*       currentAnimationState_; /**< The animation state to use while animating the skeleton */
        bool                        isLooping_;             /**< Is the animation looping? */

        vector<vector<Vector3>>     originalVertices_;  /**< Original vertices if skinning on CPU */
        vector<vector<Vector3>>     originalNormals_;   /**< Original normals if skinning on CPU */

        /**
         * Free the mesh memory
         */
        virtual void                    FreeMeshMemory();
};

}

#endif