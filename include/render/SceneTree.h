#ifndef SKETCH_3D_SCENE_TREE_H
#define SKETCH_3D_SCENE_TREE_H

#include "render/CompositeNode.h"

namespace Sketch3D {

/**
 * @class SceneTree
 * The SceneTree class is a CompositeNode that has additional functionnality
 * to render a whole scene. The actual rendering process isn't done by this
 * class; its responsability is to accumulate the geometry and transformation
 * of all its children node and send it for rendering.
 *
 * The name of this node is automatically set to "root" and it has no parent.
 */
class SceneTree : public CompositeNode {
	public:
		/**
		 * Constructor
		 */
							SceneTree();

		/**
		 * Destructor - free all children node
		 */
		virtual			   ~SceneTree();
			
		/**
		 * Render all the nodes in the tree. Note that this call doesn't
		 * immediatly render the nodes on the screen, but it prepares the
		 * rendering process.
		 */
		virtual void		Render() const;

        /**
         * Construct the parts of a node. If the part sent to this function are null, they won't be loaded.
         *  This function uses default post processing steps upon loading the files. The steps are the following:
		 *	- aiProcess_Triangulate : Triangulates all faces;
		 *	- aiProcess_GenNormals : Generates normals if none are present;
		 *	- aiProcess_GenUVCoords : Generates the UV coordinates if the texture coordinates aren't correct;
         * @param filename The name of the file from which the model should be loaded
         * @param mesh A pointer to a mesh to load the geometry. If null, then it won't be loaded
         * @param material A pointer to a material to load the textures. The material won't have any shader attached to it.
         * The user will therefore have to attach one to it to be able to render the node. If null, then the textures won't
         * be loaded.
         * @return true if the loading was succesful, false otherwise.
         */
        bool                ConstructNode(const string& filename, Mesh* mesh, Material* material) const;

        /**
         * Construct the parts of a node. If the part sent to this function are null, they won't be loaded.
         * @param filename The name of the file from which the model should be loaded
         * @param mesh A pointer to a mesh to load the geometry. If null, then it won't be loaded
         * @param prostProcessingFlags A unsigned int containing bits indicating
		 * which Assimp's post processing flags the loader should use.
         * @param material A pointer to a material to load the textures. The material won't have any shader attached to it.
         * The user will therefore have to attach one to it to be able to render the node. If null, then the textures won't
         * be loaded.
         * @return true if the loading was succesful, false otherwise.
         */
        bool                ConstructNode(const string& filename, Mesh* mesh, unsigned int postProcessingFlags, Material* material) const;
};

}

#endif