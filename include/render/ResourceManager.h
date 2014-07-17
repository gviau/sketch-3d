#ifndef SKETCH_3D_RESOURCE_MANAGER_H
#define SKETCH_3D_RESOURCE_MANAGER_H

#include "render/Texture2D.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// Forward declarations
namespace Assimp {
	class Importer;
}
struct aiMesh;
struct aiNode;
struct aiScene;

namespace Sketch3D {

/**
 * @struct LoadedModel_t
 * Structure containing data about a 3D model loaded by the manager
 */
struct LoadedModel_t {
	vector<float>			vertices;		/**< Vertices that compose the model */
	vector<float>			normals;		/**< Normals that compose the model */
	vector<float>			uvs;			/**< Texture coordinates of the model */
	vector<float>			tangent;		/**< Tangents that compose the model */
	vector<unsigned int>	indices;		/**< Indices that compose the model */
};

/**
 * @class ResourceManager
 * This class allow the user to laod a resource once and to cache it, so that
 * it can be reused quickly without loading it a second time.
 */
class ResourceManager {
	public:
		// USEFUL STATICS
		static const unsigned int		POST_PROCESS_NORMAL;
		static const unsigned int		POST_PROCESS_UV;
		static const unsigned int		POST_PROCESS_NORMAL_UV;
		static const unsigned int		POST_PROCESS_NORMAL_TANGENT;
		static const unsigned int		POST_PROCESS_NORMAL_TANGENT_UV;

		/**
		 * Destructor - free everything that it have loaded in memory
		 */
									   ~ResourceManager();

		/**
		 * Returns the singleton instance
		 */
		static ResourceManager*			GetInstance();

		/**
		 * Set the base file path to check for resources files
		 * @param filePath The file path to be used as a base path
		 */
		void							SetBaseFilePath(const string& filePath);

		/**
		 * Load a model from a file. This function uses default post processing
		 * steps upon loading the files. The steps are the following:
		 *	- aiProcess_Triangulate : Triangulates all faces;
		 *	- aiProcess_GenNormals : Generates normals if none are present;
		 *	- aiProcess_GenUVCoords : Generates the UV coordinates if the texture coordinates aren't correct;
		 * @param filename The name of the file loaded
		 * @param loadedModel A list of structures containing information about the
		 * loaded model
		 * @return true if the model was loaded correctly, false otherwise
		 */
		bool							LoadModel(const string& filename,
												  vector<LoadedModel_t*>*& loadedModel);

		/**
		 * Load a model from a file. This function accepts a third parameters
		 * which sets the Assimp post processing flags.
		 * @param filename The name of the file loaded
		 * @param prostProcessingFlags A unsigned int containing bits indicating
		 * which Assimp's post processing flags the loader should use.
		 * @param loadedModel A structure containing information about the
		 * loaded model
		 * @return true if the model was loaded correctly, false otherwise
		 */
		bool							LoadModel(const string& filename,
												  unsigned int postProcessingFlags,
												  vector<LoadedModel_t*>*& loadedModel);

		/**
		 * Load an image from a file
		 * @param filename The name of the file loaded
		 * @param texture A pointer to a texture object containing the image data
		 * @return true if the image was loaded correctly, false otherwise.
		 */
		bool							LoadTexture(const string& filename,
													Texture2D* texture);

	private:
		static ResourceManager			instance_;	/**< The singleton's instance */

		string							baseFilePath_;	/**< The base file path used by the manager to load files. */
		Assimp::Importer*				importer_;	/**< The object providing the functionnality to load models. */
		map<size_t,
            vector<LoadedModel_t*> >	models_;	/**< The cached imported models */
		map<size_t,
			pair<unsigned int, unsigned char*> >	textures_;	/**< The cached loaded textures */

        /**
         * Reserve memory in advance for the cached model data, so that we don't have to constantly
         * allocate buffers.
		 * @param scene The Assimp scene containing the meshes to load
		 * @param node The Assimp node representing the meshes to load
		 * @parram loadedModel The structure to fill
		 */
        void                            PreallocateMemory(const aiScene* scene, const aiNode* node,
                                                          vector<LoadedModel_t*>& loadedModel) const;

		/**
		 * Setup the LoadedModel_t structure from the cached model
		 * @param scene The Assimp scene containing the meshes to load
		 * @param node The Assimp node representing the meshes to load
         * @param index The index of the current mesh being loaded
		 * @parram loadedModel The structure to fill
         * @return The last index used
		 */
		int							    LoadCachedModel(const aiScene* scene, const aiNode* node, int index,
														vector<LoadedModel_t*>& loadedModel) const;

		/**
		 * Load the vertices of the mesh into the LoadedModel_t structure.
		 * @param mesh The mesh to use for loading the data
		 * @param loadedModel The structure to fill
		 */
		void							LoadMeshVertices(const aiMesh* mesh,
														 LoadedModel_t* loadedModel) const;

		/**
		 * Load the indices of the mesh into the LoadedModel_t structure.
		 * @param mesh The mesh to use for loading the data
		 * @param loadedModel The structure to fill
		 */
		void							LoadMeshIndices(const aiMesh* mesh,
														LoadedModel_t* loadedModel) const;

		/**
		 * Load the normals of the mesh into the LoadedModel_t structure.
		 * @param mesh The mesh to use for loading the data
		 * @param loadedModel The structure to fill
		 */
		void							LoadMeshNormals(const aiMesh* mesh,
														LoadedModel_t* loadedModel) const;

		/**
		 * Load the texture coordinates of the mesh into the LoadedModel_t structure.
		 * @param mesh The mesh to use for loading the data
		 * @param loadedModel The structure to fill
		 */
		void							LoadMeshTextureCoords(const aiMesh* mesh,
															  LoadedModel_t* loadedModel) const;

		/**
		 * Load the tangents of the mesh into the LoadedModel_t structure.
		 * @param mesh The mesh to use for loading the data
		 * @param loadedModel The structure to fill
		 */
		void							LoadMeshTangents(const aiMesh* mesh,
														 LoadedModel_t* loadedModel) const;

		/**
		 * Setup the Texture2D object from the cached image
		 * @param image A pair containing the DevIL image name as well as the cached data
		 * @param texture A pointer to the object to set the data to
		 */
		void							LoadCachedTexture(const pair<unsigned int, unsigned char*>& image,
														  Texture2D* texture);

		/**
		 * Constructor. Sets the base file path to "./"
		 */
										ResourceManager();

		/**
		 * Private copy-constructor - disallow copy
		 */
										ResourceManager(const ResourceManager& src);

		/**
		 * Private assignmenet operator - disallow assignment
		 */
										ResourceManager& operator= (const ResourceManager& rhs);
};

}

#endif