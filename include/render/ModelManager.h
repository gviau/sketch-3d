#ifndef SKETCH_3D_MODEL_MANAGER_H
#define SKETCH_3D_MODEL_MANAGER_H

#include "render/Mesh.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Sketch3D {

/**
 * @class ModelManager
 * This class acts as a cache for loaded models
 */
class ModelManager {
    typedef map<string, pair<int, vector<ModelSurface_t>>> CacheMap_t;

    public:
        /**
         * Destructor
         */
                               ~ModelManager();

        static ModelManager*    GetInstance();

        /**
         * Checks if the model is already loaded
         * @param filename The name of the mesh file
         * @return true if the model is already loaded, false otherwise
         */
        bool                    CheckIfModelLoaded(const string& filename) const;

        /**
         * Cache the model. This means that it is the responsability of the manager to free the memory
         * @param filename The name of the mesh file
         * @param model The list of ModelSurface_t objects representing the model
         */
        void                    CacheModel(const string& filename, const vector<ModelSurface_t>& model);

        /**
         * Load the model from the cache. This increases a pair mesh counter which tells the manager how
         * many meshes still have a reference to the model.
         * @param filename The name of the mesh file
         * @return A list of ModelSurface_t objects representing the model
         */
        vector<ModelSurface_t>  LoadFromCache(const string& filename);

        /**
         * Remove a reference from the specified cached model. When the reference count reaches 0, the model will be freed
         * @param filename The name of the model
         */
        void                    RemoveReferenceFromCache(const string& filename);

    private:
        static ModelManager     instance_;      /**< Singleton's instance */
        CacheMap_t              cachedModels_;  /**< Cached models for faster loading and reuse of data */

        /**
         * Constructor
         */
                                ModelManager();

        // Disallow copy and assignation
                                ModelManager(ModelManager& src);
        ModelManager&           operator= (ModelManager& rhs);
};

}

#endif