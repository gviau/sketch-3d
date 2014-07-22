#include "render/SceneTree.h"

#include "render/Material.h"
#include "render/Mesh.h"
#include "render/ResourceManager.h"

namespace Sketch3D {

SceneTree::SceneTree() : CompositeNode("root") {
}

SceneTree::~SceneTree() {
}

void SceneTree::Render() const {
	map<string, Node*>::const_iterator it = children_.begin();
	for (; it != children_.end(); ++it) {
		it->second->Render();
	}
}

bool SceneTree::ConstructNode(const string& filename, Mesh* mesh, Material* material) const {
    return ConstructNode(filename, mesh, ResourceManager::POST_PROCESS_NORMAL_UV, material);
}

bool SceneTree::ConstructNode(const string& filename, Mesh* mesh, unsigned int postProcessingFlags, Material* material) const {
    vector<LoadedModel_t*>* modelData;
    vector<Texture2D*>* textures;

    if (mesh && material) {
        ResourceManager::GetInstance()->LoadModel(filename, postProcessingFlags, modelData, textures);
        mesh->Initialize(modelData);
        material->SetTextures(textures);
    } else if (mesh) {
        ResourceManager::GetInstance()->LoadModelGeometryFromFile(filename, postProcessingFlags, modelData);
        mesh->Initialize(modelData);
    } else if (material) {
        ResourceManager::GetInstance()->LoadTexturesFromFile(filename, textures);
        material->SetTextures(textures);
    }

    return true;
}

}