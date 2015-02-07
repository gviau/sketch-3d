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
    typedef map<string, pair<int, Texture2D*>> TextureCacheMap_t;
    typedef map<size_t, pair<int, Texture2D**>> TextureSetCacheMap_t;

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
         * Check if a set of texture is cached
         * @param texturesFilename The textures' filename
         * @return true if those textures are cached, false otherwise
         */
        bool                    CheckIfTextureSetCached(const vector<string>& texturesFilename) const;

        /**
         * Cache the texture. This means that it is the responsability of the manager to free the memory.
         * The user have to check if the texture is already cached or not before calling this function
         * @param filename The name of the texture file
         * @param texture A pointer to the textture object
         */
        void                    CacheTexture(const string& filename, Texture2D* texture);

        /**
         * Cache a set of textures. This means that it is the responsability of the manage to free the memory.
         * The user have to check if the texture set is already cached or not before calling this function
         * @param texturesFilename The textures' filename
         * @param textures The actual list of textures to cache
         */
        void                    CacheTextureSet(const vector<string>& texturesFilename, Texture2D** textures);

        /**
         * Load the texture from the cache. This increases a counter which tells the manager how
         * many textures still hold a reference. The user have to check if the texture is already cached or not before calling this function
         * @param filename The name of the texture file
         * @return A pointer to the texture object
         */
        Texture2D*              LoadTextureFromCache(const string& filename);

        /**
         * Load a texture set from the cache. This increases a counter which tells the manager how many times the
         * texture set have been referenced. The user have to check if the texture set is already cached or not before calling this function
         * @param texturesFilename The textures' filename
         * @return A pointer to the list of textures
         */
        Texture2D**             LoadTextureSetFromCache(const vector<string>& texturesFilename);

        /**
         * Remove a reference from the specified cached texture. When the reference count reaches 0, the texture will be freed.
         * The user have to check if the texture is already cached or not before calling this function
         * @param filename The name of the texture
         */
        void                    RemoveTextureReferenceFromCache(const string& filename);

    private:
        static TextureManager   instance_;          /**< Singleton's instance */
        TextureCacheMap_t       cachedTextures_;    /**< Cached textures for faster loading and reuse of data */
        TextureSetCacheMap_t    cachedTextureSets_; /**< Cached texture sets for reuse of data */

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