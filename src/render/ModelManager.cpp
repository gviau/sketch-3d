#include "render/ModelManager.h"

#include "system/Logger.h"

namespace Sketch3D {

ModelManager ModelManager::instance_;

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
    ModelCacheMap_t::iterator m_it = cachedModels_.begin();
    for (; m_it != cachedModels_.end(); ++m_it) {
        vector<ModelSurface_t>& models = m_it->second.second;

        for (size_t i = 0; i < models.size(); i++) {
            ModelSurface_t model = models[i];
            delete[] model.geometry->vertices;
            delete[] model.geometry->normals;
            delete[] model.geometry->texCoords;
            delete[] model.geometry->tangents;
            delete[] model.geometry->indices;
            delete[] model.geometry->textures;
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

void ModelManager::CacheModel(const string& filename, const vector<ModelSurface_t>& model) {
    if (!CheckIfModelLoaded(filename)) {
        cachedModels_[filename] = pair<int, vector<ModelSurface_t>>(1, model);
    }
}

void ModelManager::CacheSkeleton(const string& filename, Skeleton* skeleton) {
    if (!CheckIfSkeletonLoaded(filename)) {
        cachedSkeletons_[filename] = pair<int, Skeleton*>(1, skeleton);
    }
}

vector<ModelSurface_t> ModelManager::LoadModelFromCache(const string& filename) {
    vector<ModelSurface_t> model = cachedModels_[filename].second;
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
        vector<ModelSurface_t>& models = cachedModels_[filename].second;

        for (size_t i = 0; i < models.size(); i++) {
            ModelSurface_t& model = models[i];
            delete[] model.geometry->vertices;
            delete[] model.geometry->normals;
            delete[] model.geometry->texCoords;
            delete[] model.geometry->tangents;
            delete[] model.geometry->indices;
            delete[] model.geometry->textures;
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