#include "render/ModelManager.h"

#include "render/Texture2D.h"
#include "render/TextureManager.h"

#include "system/Logger.h"

#include <set>
using namespace std;

namespace Sketch3D {

ModelManager ModelManager::instance_;

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
    ModelCacheMap_t::iterator m_it = cachedModels_.begin();
    for (; m_it != cachedModels_.end(); ++m_it) {
        vector<SurfaceTriangles_t*>& models = m_it->second.second;

        for (size_t i = 0; i < models.size(); i++) {
            SurfaceTriangles_t* surface = models[i];

            delete[] surface->vertices;
            delete[] surface->normals;
            delete[] surface->texCoords;
            delete[] surface->tangents;
            delete[] surface->bones;
            delete[] surface->weights;
            delete[] surface->indices;

            // We let the TextureManager take care of freeing the textures pointer
        }

        Logger::GetInstance()->Info("Model \"" + m_it->first + "\" freed");
    }

    SkeletonCacheMap_t::iterator s_it = cachedSkeletons_.begin();
    for (; s_it != cachedSkeletons_.end(); ++s_it) {
        Skeleton* skeleton = s_it->second.second;
        delete skeleton;

        Logger::GetInstance()->Info("Skeleton of model \"" + s_it->first + "\" freed");
    }
}

ModelManager* ModelManager::GetInstance() {
    return &instance_;
}

bool ModelManager::CheckIfModelLoaded(const string& filename) const {
    ModelCacheMap_t::const_iterator it = cachedModels_.find(filename);
    return (it != cachedModels_.end());
}

bool ModelManager::CheckIfSkeletonLoaded(const string& filename) const {
    SkeletonCacheMap_t::const_iterator it = cachedSkeletons_.find(filename);
    return (it != cachedSkeletons_.end());
}

void ModelManager::CacheModel(const string& filename, const vector<SurfaceTriangles_t*>& model) {
    cachedModels_[filename] = pair<int, vector<SurfaceTriangles_t*>>(1, model);
}

void ModelManager::CacheSkeleton(const string& filename, Skeleton* skeleton) {
    cachedSkeletons_[filename] = pair<int, Skeleton*>(1, skeleton);
}

vector<SurfaceTriangles_t*> ModelManager::LoadModelFromCache(const string& filename) {
    vector<SurfaceTriangles_t*> model = cachedModels_[filename].second;
    cachedModels_[filename].first += 1;
    return model;
}

Skeleton* ModelManager::LoadSkeletonFromCache(const string& filename) {
    Skeleton* skeleton = cachedSkeletons_[filename].second;
    cachedSkeletons_[filename].first += 1;
    return skeleton;
}

void ModelManager::RemoveModelReferenceFromCache(const string& filename) {
    cachedModels_[filename].first -= 1;
    if (cachedModels_[filename].first == 0) {
        vector<SurfaceTriangles_t*>& models = cachedModels_[filename].second;
        set<Texture2D*> texturesToDelete;

        for (size_t i = 0; i < models.size(); i++) {
            SurfaceTriangles_t* surface = models[i];

            delete[] surface->vertices;
            delete[] surface->normals;
            delete[] surface->texCoords;
            delete[] surface->tangents;
            delete[] surface->indices;

            for (size_t j = 0; j < surface->numTextures; j++) {
                Texture2D* texture = surface->textures[j];
                texturesToDelete.insert(texture);
            }

            // We let the TextureManager take care of freeing the textures pointer
        }

        set<Texture2D*>::iterator it = texturesToDelete.begin();
        for (; it != texturesToDelete.end(); ++it) {
            Texture2D* texture = *it;

            if (texture != nullptr) {
                if (TextureManager::GetInstance()->CheckIfTextureLoaded(texture->GetFilename())) {
                    TextureManager::GetInstance()->RemoveTextureReferenceFromCache(texture->GetFilename());
                }
            }
        }

        cachedModels_.erase(filename);
    }
}

void ModelManager::RemoveSkeletonReferenceFromCache(const string& filename) {
    cachedSkeletons_[filename].first -= 1;
    if (cachedSkeletons_[filename].first == 0) {
        Skeleton* skeleton = cachedSkeletons_[filename].second;
        delete skeleton;

        cachedSkeletons_.erase(filename);
    }
}

}