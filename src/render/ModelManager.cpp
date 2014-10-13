#include "render/ModelManager.h"

namespace Sketch3D {

ModelManager ModelManager::instance_;

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
    CacheMap_t::iterator it = cachedModels_.begin();
    for (; it != cachedModels_.end(); ++it) {
        vector<ModelSurface_t>& models = it->second.second;

        for (size_t i = 0; i < models.size(); i++) {
            ModelSurface_t model = models[i];
            delete[] model.geometry->vertices;
            delete[] model.geometry->normals;
            delete[] model.geometry->texCoords;
            delete[] model.geometry->tangents;
            delete[] model.geometry->indices;
            delete[] model.geometry->textures;
        }
    }
}

ModelManager* ModelManager::GetInstance() {
    return &instance_;
}

bool ModelManager::CheckIfModelLoaded(const string& filename) const {
    CacheMap_t::const_iterator it = cachedModels_.find(filename);
    return (it != cachedModels_.end());
}

void ModelManager::CacheModel(const string& filename, const vector<ModelSurface_t>& model) {
    if (!CheckIfModelLoaded(filename)) {
        cachedModels_[filename] = pair<int, vector<ModelSurface_t>>(1, model);
    }
}

vector<ModelSurface_t> ModelManager::LoadFromCache(const string& filename) {
    vector<ModelSurface_t> model = cachedModels_[filename].second;
    cachedModels_[filename].first += 1;
    return model;
}

void ModelManager::RemoveReferenceFromCache(const string& filename) {
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

}