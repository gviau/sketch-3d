#include "render/TextureManager.h"

#include "render/Texture2D.h"
#include "system/Logger.h"

namespace Sketch3D {

TextureManager TextureManager::instance_;

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
    CacheMap_t::iterator it = cachedTextures_.begin();
    for ( ; it != cachedTextures_.end(); ) {
        string filename = it->first;
        Texture2D* texture = it->second.second;
        cachedTextures_.erase(it++);
        delete texture;
        Logger::GetInstance()->Info("Image \"" + filename + "\" freed");
    }
}

TextureManager* TextureManager::GetInstance() {
    return &instance_;
}

bool TextureManager::CheckIfTextureLoaded(const string& filename) const {
    CacheMap_t::const_iterator it = cachedTextures_.find(filename);
    return (it != cachedTextures_.end());
}

void TextureManager::CacheTexture(const string& filename, Texture2D* texture) {
    if (!CheckIfTextureLoaded(filename)) {
        cachedTextures_[filename] = pair<int, Texture2D*>(1, texture);
    }
}

Texture2D* TextureManager::LoadFromCache(const string& filename) {
    Texture2D* texture = cachedTextures_[filename].second;
    cachedTextures_[filename].first += 1;
    return texture;
}

void TextureManager::RemoveReferenceFromCache(const string& filename) {
    if (!CheckIfTextureLoaded(filename)) {
        return;
    }

    cachedTextures_[filename].first -= 1;
    if (cachedTextures_[filename].first == 0) {
        delete cachedTextures_[filename].second;
        cachedTextures_.erase(filename);
    }
}

}