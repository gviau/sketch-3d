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
#include <memory>
#include <set>
#include <utility>
using namespace std;

namespace Sketch3D {

/**
 * @enum RenderCommand_t
 * This enum defines the different rendering command used by the render queue to actually draw
 * on the screen. The commands are used in an array of commands to sequentially make API calls.
 */
enum RenderCommand_t {
    RENDER_COMMAND_USE_MATERIAL,
    RENDER_COMMAND_BIND_TEXTURES,
    RENDER_COMMAND_SET_MODEL_MATRIX,
    RENDER_COMMAND_RENDER_BUFFER_OBJECTS,
    RENDER_COMMAND_ACCUMULATE_MODEL_MATRIX,
    RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES,

    NUMBER_RENDER_COMMANDS
};

/**
 * @struct BindTextures_t
 * Little struct used to pack information required textures that we have to bind
 */
struct BindTextures_t {
    size_t      numTextures;
    Texture2D** textures;

    // Required to be used in a set
    bool operator<(const BindTextures_t& rhs) const {
        return textures < rhs.textures;
    }
};

RenderQueue::RenderQueue() {
}

void RenderQueue::AddNode(Node* node, Layer_t layer) {
    // TODO
    // Got to change the distance from the camera
    BufferObject** bufferObjects;
    vector<SurfaceTriangles_t*> surfaces;
    node->GetMesh()->GetRenderInfo(bufferObjects, surfaces);
    shared_ptr<Matrix4x4> model(new Matrix4x4(node->ConstructModelMatrix()));

    const Matrix4x4& modelView = Renderer::GetInstance()->GetViewMatrix() * *(model.get());
    float dist = -(modelView[2][3] + Renderer::GetInstance()->GetNearFrustumPlane()) / (Renderer::GetInstance()->GetFarFrustumPlane() - Renderer::GetInstance()->GetNearFrustumPlane());
    uint32_t distanceToCamera = (uint32_t)(dist * (float)UINT32_MAX);

    for (size_t i = 0; i < surfaces.size(); i++) {
        items_.push_back(RenderQueueItem(model, node->GetMaterial(), surfaces[i]->textures,
                         surfaces[i]->numTextures, bufferObjects[i], node->UseInstancing(), distanceToCamera, layer));

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

    vector<pair<RenderCommand_t, void*>> renderCommands;
    set<BindTextures_t> bindTexturesSet;
    set<void*> instancedBufferObjects;

    // This array is used to determine when to insert a new render command in the list of render commands
    void* previousRenderCommands[NUMBER_RENDER_COMMANDS];
    for (size_t i = 0; i < NUMBER_RENDER_COMMANDS; i++) {
        previousRenderCommands[i] = nullptr;
    }
    bool modelViewMatrixChanged = false;
    bool nextRenderIsInstanced = false;

    // Construct the list of render commands
    for (size_t i = 0; i < itemsIndex_.size(); i++) {
        size_t idx = itemsIndex_[i];
        const RenderQueueItem& item = items_[idx];

        // Set the material to bind for the next render commands
        void* material = static_cast<void*>(item.material_);
        if (previousRenderCommands[RENDER_COMMAND_USE_MATERIAL] != material) {

            // Flush the current batch of instanced buffers before we switch material
            if (nextRenderIsInstanced) {
                set<void*>::iterator it = instancedBufferObjects.begin();
                for (; it != instancedBufferObjects.end(); ++it) {
                    renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES, *it));
                }
                instancedBufferObjects.clear();

                nextRenderIsInstanced = item.useInstancing_;
            }

            renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_USE_MATERIAL, material));
            previousRenderCommands[RENDER_COMMAND_USE_MATERIAL] = material;
        }

        // If any, set the textures to bind for the next render commands
        if (item.numTextures_ > 0) {
            BindTextures_t bindTextures;
            bindTextures.numTextures = item.numTextures_;
            bindTextures.textures = item.textures_;
            pair<set<BindTextures_t>::iterator, bool> ret = bindTexturesSet.insert(bindTextures);
        
            void* textures = static_cast<void*>(const_cast<BindTextures_t*>(&(*(ret.first)))); // Evil stuff to make it work :/
            if (previousRenderCommands[RENDER_COMMAND_BIND_TEXTURES] != textures) {

                // Flush the current batch of instanced buffers before we switch textures
                if (nextRenderIsInstanced) {
                    set<void*>::iterator it = instancedBufferObjects.begin();
                    for (; it != instancedBufferObjects.end(); ++it) {
                        renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES, *it));
                    }
                    instancedBufferObjects.clear();

                    nextRenderIsInstanced = item.useInstancing_;
                }

                renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_BIND_TEXTURES, textures));
                previousRenderCommands[RENDER_COMMAND_BIND_TEXTURES] = textures;
            }
        }

        // Set the model matrix for the next render commands. If we are using instanced rendering, we want to accumulate them, instead
        void* modelMatrix = static_cast<void*>(item.modelMatrix_.get());
        if (previousRenderCommands[RENDER_COMMAND_SET_MODEL_MATRIX] != modelMatrix) {

            if (item.useInstancing_) {
                renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_ACCUMULATE_MODEL_MATRIX, modelMatrix));
                nextRenderIsInstanced = true;
            } else {

                // Flush the current batch of instanced buffers
                if (nextRenderIsInstanced) {
                    set<void*>::iterator it = instancedBufferObjects.begin();
                    for (; it != instancedBufferObjects.end(); ++it) {
                        renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES, *it));
                    }
                    instancedBufferObjects.clear();

                    nextRenderIsInstanced = false;
                }

                renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_SET_MODEL_MATRIX, modelMatrix));
            }

            modelViewMatrixChanged = true;
            previousRenderCommands[RENDER_COMMAND_SET_MODEL_MATRIX] = modelMatrix;
        }

        // Set the buffer objects to draw. If we are using instanced rendering, we want to delay their insertion in the list of
        // render commands to after we accumulated all the model matrices
        void* bufferObject = static_cast<void*>(item.bufferObject_);
        if (modelViewMatrixChanged || previousRenderCommands[RENDER_COMMAND_RENDER_BUFFER_OBJECTS] != bufferObject) {

            if (item.useInstancing_) {
                instancedBufferObjects.insert(bufferObject);
                nextRenderIsInstanced = true;
            } else {

                // Flush the current batch of instanced buffers
                if (nextRenderIsInstanced) {
                    set<void*>::iterator it = instancedBufferObjects.begin();
                    for (; it != instancedBufferObjects.end(); ++it) {
                        renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES, *it));
                    }
                    instancedBufferObjects.clear();

                    nextRenderIsInstanced = false;
                }

                renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_RENDER_BUFFER_OBJECTS, bufferObject));
            }

            previousRenderCommands[RENDER_COMMAND_RENDER_BUFFER_OBJECTS] = bufferObject;
            modelViewMatrixChanged = false;
        }
    }

    // Render the last batch of instanced buffer objects if we didn't have the chance to flush them out
    if (nextRenderIsInstanced) {
        set<void*>::iterator it = instancedBufferObjects.begin();
        for (; it != instancedBufferObjects.end(); ++it) {
            renderCommands.push_back(pair<RenderCommand_t, void*>(RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES, *it));
        }
    }

    // Execute the commands sequentially
    const Material* currentMaterial = nullptr;
    const BindTextures_t* currentTextures = nullptr;
    const Matrix4x4* currentModelMatrix;
    BufferObject* currentBufferObject = nullptr;
    Shader* currentShader = nullptr;
    const map<string, Texture*>* currentMaterialTextures = nullptr;
    map<string, Texture*>::const_iterator mt_it;
    vector<Matrix4x4> accumulatedInstances;
    bool flushAccumulatedInstances = false;

    const Matrix4x4& projection = Renderer::GetInstance()->GetProjectionMatrix();
    const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
    const Matrix4x4& view = Renderer::GetInstance()->GetViewMatrix();
    const Matrix4x4& transposedInverseViewMatrix = view.Inverse().Transpose();

    for (size_t i = 0; i < renderCommands.size(); i++) {
        RenderCommand_t renderCommand = renderCommands[i].first;

        switch (renderCommand) {
            case RENDER_COMMAND_USE_MATERIAL:
                currentMaterial = static_cast<Material*>(renderCommands[i].second);
                currentShader = currentMaterial->GetShader();

                // Bind the current shader for all the following draw calls
                Renderer::GetInstance()->BindShader(currentShader);
                currentShader->SetUniformMatrix4x4("view", view);
                currentShader->SetUniformMatrix4x4("viewProjection", viewProjection);
                currentShader->SetUniformMatrix4x4("transInvView", transposedInverseViewMatrix);
                currentShader->SetUniformMatrix4x4("projection", projection);

                // Material's textures
                currentMaterialTextures = &(currentMaterial->GetTextures());
                mt_it = currentMaterialTextures->begin();

                for (; mt_it != currentMaterialTextures->end(); ++mt_it) {
                    if (mt_it->second != nullptr) {
                        currentShader->SetUniformTexture(mt_it->first, mt_it->second);
                    }
                }

                break;

            case RENDER_COMMAND_BIND_TEXTURES:
                // Bind the textures for the next sets of buffer objects
                currentTextures = static_cast<BindTextures_t*>(renderCommands[i].second);

                for (size_t j = 0; j < currentTextures->numTextures; j++) {
                    Texture2D* texture = currentTextures->textures[j];
                    if (texture != nullptr) {
                        currentShader->SetUniformTexture("texture" + to_string(j), texture);
                    }
                }
                break;

            case RENDER_COMMAND_SET_MODEL_MATRIX:
                // Setup the transformation matrix for the next sets of buffer objects
                currentModelMatrix = static_cast<Matrix4x4*>(renderCommands[i].second);

                // Set the uniform matrices
                currentShader->SetUniformMatrix4x4("modelViewProjection", viewProjection * (*currentModelMatrix));
                currentShader->SetUniformMatrix4x4("modelView", view * (*currentModelMatrix));
                currentShader->SetUniformMatrix4x4("model", (*currentModelMatrix));
                currentShader->SetUniformMatrix4x4("transInvModelView", transposedInverseViewMatrix * currentModelMatrix->Inverse().Transpose());
                break;

            case RENDER_COMMAND_RENDER_BUFFER_OBJECTS:
                // Draw a buffer object
                currentBufferObject = static_cast<BufferObject*>(renderCommands[i].second);
                currentBufferObject->Render();
                break;

            case RENDER_COMMAND_ACCUMULATE_MODEL_MATRIX:
                // Accumulate the model matrices
                if (flushAccumulatedInstances) {
                    accumulatedInstances.clear();
                    flushAccumulatedInstances = false;
                }

                currentModelMatrix = static_cast<Matrix4x4*>(renderCommands[i].second);
                accumulatedInstances.push_back(currentModelMatrix->Transpose());
                break;

            case RENDER_COMMAND_DRAW_ACCUMULATED_INSTANCES:
                // Draw several instances of the same buffer object
                currentBufferObject = static_cast<BufferObject*>(renderCommands[i].second);
                currentBufferObject->RenderInstances(accumulatedInstances);
                flushAccumulatedInstances = true;

                break;
        }
    }

    // Invalidate the render queue and approximate the next batch's size
    items_.clear();
    items_.reserve(itemsIndex_.size());
}

bool RenderQueue::IsEmpty() const {
    return items_.empty();
}

}
