#include "render/TextureManager.h"

#include "render/Texture2D.h"
#include "system/Logger.h"

namespace Sketch3D {

TextureManager TextureManager::instance_;

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
    TextureCacheMap_t::iterator t_it = cachedTextures_.begin();
    for ( ; t_it != cachedTextures_.end();) {
        string filename = t_it->first;
        Texture2D* texture = t_it->second.second;
        delete texture;
        cachedTextures_.erase(t_it++);
        Logger::GetInstance()->Info("Image \"" + filename + "\" freed");
    }

    TextureSetCacheMap_t::iterator ts_it = cachedTextureSets_.begin();
    for ( ; ts_it != cachedTextureSets_.end(); ++ts_it) {
        Texture2D** textures = ts_it->second.second;
        delete[] textures;
    }
}

TextureManager* TextureManager::GetInstance() {
    return &instance_;
}

bool TextureManager::CheckIfTextureLoaded(const string& filename) const {
    TextureCacheMap_t::const_iterator it = cachedTextures_.find(filename);
    return (it != cachedTextures_.end());
}

bool TextureManager::CheckIfTextureSetCached(const vector<string>& texturesFilename) const {
    std::hash<string> stringHash;
    string concatenation;

    for (size_t i = 0; i < texturesFilename.size(); i++) {
        concatenation += texturesFilename[i];
    }
    size_t hashValue = stringHash(concatenation);

    return (cachedTextureSets_.find(hashValue) != cachedTextureSets_.end());
}

void TextureManager::CacheTexture(const string& filename, Texture2D* texture) {
    cachedTextures_[filename] = pair<int, Texture2D*>(1, texture);
}

void TextureManager::CacheTextureSet(const vector<string>& texturesFilename, Texture2D** textures) {
    std::hash<string> stringHash;
    string concatenation;

    for (size_t i = 0; i < texturesFilename.size(); i++) {
        concatenation += texturesFilename[i];
    }
    size_t hashValue = stringHash(concatenation);

    cachedTextureSets_[hashValue] = pair<int, Texture2D**>(1, textures);
}

Texture2D* TextureManager::LoadTextureFromCache(const string& filename) {
    Texture2D* texture = cachedTextures_[filename].second;
    cachedTextures_[filename].first += 1;
    return texture;
}

Texture2D** TextureManager::LoadTextureSetFromCache(const vector<string>& texturesFilename) {
    std::hash<string> stringHash;
    string concatenation;

    for (size_t i = 0; i < texturesFilename.size(); i++) {
        concatenation += texturesFilename[i];
    }
    size_t hashValue = stringHash(concatenation);

    Texture2D** textures = cachedTextureSets_[hashValue].second;
    cachedTextureSets_[hashValue].first += 1;

    return textures;
}

void TextureManager::RemoveTextureReferenceFromCache(const string& filename) {
    cachedTextures_[filename].first -= 1;
    if (cachedTextures_[filename].first == 0) {
        delete cachedTextures_[filename].second;
        cachedTextures_.erase(filename);
    }
}

}