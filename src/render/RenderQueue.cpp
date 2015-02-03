#include "render/RenderQueue.h"

#include "render/BufferObject.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "render/Renderer.h"
#include "render/Shader.h"
#include "render/SkinnedMesh.h"
#include "render/Texture2D.h"

#include <algorithm>
#include <utility>
using namespace std;

namespace Sketch3D {

/**
 * @struct RenderGroup_t
 * Structure containing containing the actual rendering data after sorting the render queue
 */
struct RenderGroup_t {
    Texture2D**             textures;
    size_t                  numTextures;
    vector<BufferObject*>   bufferObjects;
    vector<Matrix4x4>       transformations;
};

RenderQueue::RenderQueue() {
}

void RenderQueue::AddNode(const Node* node, Layer_t layer) {
    // TODO
    // Got to change the distance from the camera
    BufferObject** bufferObjects;
    vector<ModelSurface_t> surfaces;
    node->GetMesh()->GetRenderInfo(bufferObjects, surfaces);
    
    for (size_t i = 0; i < surfaces.size(); i++) {
        items_.push_back(RenderQueueItem(node->ConstructModelMatrix(), node->GetMaterial(), surfaces[i].geometry->textures,
                                         surfaces[i].geometry->numTextures, bufferObjects[i], 0, layer));

        if (items_.size() > itemsIndex_.size()) {
            itemsIndex_.push_back(itemsIndex_.size());
        }
    }
}

void RenderQueue::Render() {
    // Do we have less index this frame?
    if (items_.size() < itemsIndex_.size()) {
        itemsIndex_.resize(items_.size());

        for (size_t i = 0; i < itemsIndex_.size(); i++) {
            itemsIndex_[i] = i;
        }
    }

    // Sort the indices
    stable_sort(itemsIndex_.begin(), itemsIndex_.end(), RenderQueueItemKeyComparator(items_));

    // For group of identical objects, that is if they have the exact same mesh and exact same material so
    // that we can batch them
    vector<pair<const Material*, vector<RenderGroup_t>>> renderGroups;
    map<const Material*, size_t> materialIndexMap;
    map<const Material*, map<Texture2D**, size_t>> textureIndexMap;

    for (size_t i = 0; i < itemsIndex_.size(); i++) {
        size_t idx = itemsIndex_[i];
        const RenderQueueItem& item = items_[idx];

        map<const Material*, size_t>::iterator m_it = materialIndexMap.find(item.material_);
        size_t materialIndex = 0;
        size_t textureIndex = 0;

        if (m_it == materialIndexMap.end()) {
            materialIndexMap[item.material_] = renderGroups.size();
            materialIndex = materialIndexMap[item.material_];
            renderGroups.push_back(pair<const Material*, vector<RenderGroup_t>>(item.material_, vector<RenderGroup_t>()));
            
            textureIndexMap[item.material_][item.textures_] = renderGroups[materialIndex].second.size();

            RenderGroup_t renderGroup;
            renderGroup.bufferObjects.push_back(item.bufferObject_);
            renderGroup.textures = item.textures_;
            renderGroup.numTextures = item.numTextures_;
            renderGroups[materialIndex].second.push_back(renderGroup);
        } else {
            materialIndex = materialIndexMap[item.material_];
        }

        map<Texture2D**, size_t>::iterator t_it = textureIndexMap[item.material_].find(item.textures_);
        if (t_it == textureIndexMap[item.material_].end()) {
            textureIndexMap[item.material_][item.textures_] = renderGroups[materialIndex].second.size();
            textureIndex = textureIndexMap[item.material_][item.textures_];

            RenderGroup_t renderGroup;
            renderGroup.bufferObjects.push_back(item.bufferObject_);
            renderGroup.textures = item.textures_;
            renderGroup.numTextures = item.numTextures_;
            renderGroups[materialIndex].second.push_back(renderGroup);
        } else {
            textureIndex = textureIndexMap[item.material_][item.textures_];
        }

        renderGroups[materialIndex].second[textureIndex].transformations.push_back(item.modelMatrix_);
    }

    const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
    const Matrix4x4& view = Renderer::GetInstance()->GetViewMatrix();

    // Draw the render groups
    for (size_t i = 0; i < renderGroups.size(); i++) {
        const pair<const Material*, vector<RenderGroup_t>>& materialRenderGroups = renderGroups[i];
        const Material* material = materialRenderGroups.first;
        const vector<RenderGroup_t>& groups = materialRenderGroups.second;

        // Bind the shader for all following draw calls
        Shader* shader = material->GetShader();
        Renderer::GetInstance()->BindShader(shader);

        shader->SetUniformMatrix4x4("view", Renderer::GetInstance()->GetViewMatrix());

        // Material's textures
        const map<string, Texture*>& materialTextures = material->GetTextures();
        map<string, Texture*>::const_iterator it = materialTextures.begin();

        for (; it != materialTextures.end(); ++it) {
            if (it->second != nullptr) {
                shader->SetUniformTexture(it->first, it->second);
            }
        }

        // For each set of textures
        for (size_t j = 0; j < groups.size(); j++) {
            const RenderGroup_t& renderGroup = groups[j];

            for (size_t k = 0; k < renderGroup.numTextures; k++) {
                Texture2D* texture = renderGroup.textures[k];
                if (texture != nullptr) {
                    shader->SetUniformTexture("texture" + to_string(k), texture);
                }
            }

            // For all different transformations applying to the same buffer object
            for (size_t k = 0; k < renderGroup.transformations.size(); k++) {
                const Matrix4x4& model = renderGroup.transformations[k];

                // Setup the transformation matrix for this node
                Matrix4x4 modelViewProjection(viewProjection * model);
                Matrix4x4 modelView(view * model);

                // Set the uniform matrices
                shader->SetUniformMatrix4x4("modelViewProjection", modelViewProjection);
                shader->SetUniformMatrix4x4("modelView", modelView);
                shader->SetUniformMatrix4x4("model", model);

                // Render the BufferObjects
                for (size_t l = 0; l < renderGroup.bufferObjects.size(); l++) {
                    renderGroup.bufferObjects[l]->Render();
                }
            }
        }
    }

    // Invalidate the render queue
    items_.clear();
}

}
