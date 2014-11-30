#ifndef SKETCH_3D_MODEL_MANAGER_H
#define SKETCH_3D_MODEL_MANAGER_H

#include "render/Mesh.h"
#include "render/Skeleton.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @class ModelManager
 * This class acts as a cache for loaded models
 */
class ModelManager {
    typedef map<string, pair<int, vector<ModelSurface_t>>> ModelCacheMap_t;
    typedef map<string, pair<int, Skeleton*>> SkeletonCacheMap_t;

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
         * Checks if the skeleton for the model is already loaded
         * @param filename The name of the mesh file
         * @return true if the skeleton for that model is already loaded, false otherwise
         */
        bool                    CheckIfSkeletonLoaded(const string& filename) const;

        /**
         * Cache the model. This means that it is the responsability of the manager to free the memory
         * @param filename The name of the mesh file
         * @param model The list of ModelSurface_t objects representing the model
         */
        void                    CacheModel(const string& filename, const vector<ModelSurface_t>& model);

        /**
         * Cache the skeleton for the model. This means that it is the responsability of the manager to free the
         * memory.
         * @param filename The name of the mesh file
         * @param skeleton The skeleton of the model
         */
        void                    CacheSkeleton(const string& filename, Skeleton* bones);

        /**
         * Load the model from the cache. This increases a pair mesh counter which tells the manager how
         * many meshes still have a reference to the model.
         * @param filename The name of the mesh file
         * @return A list of ModelSurface_t objects representing the model
         */
        vector<ModelSurface_t>  LoadModelFromCache(const string& filename);

        /**
         * Load the skeleton of the model from the cache. This increases a counter which tells the manager how
         * many times the skeleton have been referenced for that model
         * @param filename The name of the mesh file
         * @return A pointer representing the skeleton of the model
         */
        Skeleton*               LoadSkeletonFromCache(const string& filename);

        /**
         * Remove a reference from the specified cached model. When the reference count reaches 0, the model will be freed
         * @param filename The name of the model
         */
        void                    RemoveModelReferenceFromCache(const string& filename);

        /**
         * Remove a reference for the skeleton of the specified cached model. when the reference count reaches 0, the skeleton will
         * be freed.
         * @param filename The name of the model
         */
        void                    RemoveSkeletonReferenceFromCache(const string& filename);

    private:
        static ModelManager     instance_;      /**< Singleton's instance */
        ModelCacheMap_t         cachedModels_;  /**< Cached models for faster loading and reuse of data */
        SkeletonCacheMap_t      cachedSkeletons_;   /**< Cached skeletons for the models */

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