#ifndef SKETCH_3D_TEXTURE_MANAGER_H
#define SKETCH_3D_TEXTURE_MANAGER_H

#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

namespace Sketch3D {
// Forward declaration
class Texture2D;

/**
 * @class TextureManager
 * This class acts as a cache for loaded textures
 */
class TextureManager {
    typedef map<string, pair<int, Texture2D*>> CacheMap_t;

    public:
        /**
         * Destructor
         */
                               ~TextureManager();

        static TextureManager*  GetInstance();

        /**
         * Checks if the texture is already loaded
         * @param filename The name of the texture file
         * @return true if the texture is already loaded, false otherwise
         */
        bool                    CheckIfTextureLoaded(const string& filename) const;

        /**
         * Cache the texture. This means that it is the responsability of the manager to free the memory
         * @param filename The name of the texture file
         * @param texture A pointer to the textture object
         */
        void                    CacheTexture(const string& filename, Texture2D* texture);

        /**
         * Load the texture from the cache. This increases a counter which tells the manager how
         * many textures still hold a reference.
         * @param filename The name of the texture file
         * @return A pointer to the texture object
         */
        Texture2D*              LoadFromCache(const string& filename);

        /**
         * Remove a reference from the specified cached texture. When the reference count reaches 0, the texture will be freed
         * @param filename The name of the texture
         */
        void                    RemoveReferenceFromCache(const string& filename);

    private:
        static TextureManager   instance_;      /**< Singleton's instance */
        CacheMap_t              cachedTextures_;  /**< Cached textures for faster loading and reuse of data */

        /**
         * Constructor
         */
                                TextureManager();

        // Disallow copy and assignation
                                TextureManager(TextureManager& src);
        TextureManager&         operator= (TextureManager& rhs);
};

}

#endif