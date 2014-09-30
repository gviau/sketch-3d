#include "render/RenderQueue.h"

#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "render/Renderer.h"
#include "render/Shader.h"

#include <algorithm>
using namespace std;

// TEMP
#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {

RenderQueue::RenderQueue() {
}

void RenderQueue::AddItem(const Node& node, Layer_t layer) {
	const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
	Matrix4x4 model;

	// Setup the transformation matrix for this node
    Vector3 scale = node.GetScale();
	model[0][0] = scale.x;
	model[1][1] = scale.y;
	model[2][2] = scale.z;

	Matrix4x4 rotation;
    node.GetOrientation().ToRotationMatrix(rotation);
	model = rotation * model;

    Vector3 position = node.GetPosition();
	model[0][3] = position.x;
	model[1][3] = position.y;
	model[2][3] = position.z;

    Matrix4x4 fullModelView = Renderer::GetInstance()->GetViewMatrix() * model;
    Matrix3x3* modelView = new Matrix3x3(fullModelView);

    float distanceFromCamera = fullModelView[2][3];
    items_.push_back(new RenderQueueItem(node.GetMesh(), node.GetMaterial(), distanceFromCamera, layer));

    RenderQueueItem* item = items_[items_.size() - 1];

    // TODO
    // Find a better way. This is kind of a ugly hack
    // Set uniforms
    Matrix4x4* modelViewProjection = new Matrix4x4(viewProjection * model);

    item->uniforms_["modelViewProjection"] = pair<UniformType_t, void*>(UNIFORM_TYPE_MATRIX4X4, (void*)modelViewProjection);
    item->uniforms_["modelView"] = pair<UniformType_t, void*>(UNIFORM_TYPE_MATRIX3X3, (void*)modelView);

    // TODO add uniforms from material
}

void RenderQueue::Render() {
    // Sort
    stable_sort(items_.begin(), items_.end(), RenderQueueItemKeyComparator());

    // Batch if possible

    for (size_t i = 0; i < items_.size(); i++) {
        RenderQueueItem* item = items_[i];
    
        // Retrieve uniforms from map, set them to the shader program
        Shader* shader = item->material_->GetShader();
        shader->SetActive(true);

        RenderQueueItem::UniformMap_t::iterator it = item->uniforms_.begin();
        for (; it != item->uniforms_.end(); ++it) {
            pair<UniformType_t, void*> uniform = it->second;

            float* val = nullptr;
            switch (uniform.first) {
                case UNIFORM_TYPE_INT:
                    shader->SetUniformInt(it->first, *(reinterpret_cast<int*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_FLOAT:
                    shader->SetUniformFloat(it->first, *(reinterpret_cast<float*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_VECTOR2:
                    val = reinterpret_cast<float*>(uniform.second);
                    shader->SetUniformVector2(it->first, val[0], val[1]);
                    break;

                case UNIFORM_TYPE_VECTOR3:
                    shader->SetUniformVector3(it->first, *(reinterpret_cast<Vector3*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_VECTOR4:
                    shader->SetUniformVector4(it->first, *(reinterpret_cast<Vector4*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_MATRIX3X3:
                    shader->SetUniformMatrix3x3(it->first, *(reinterpret_cast<Matrix3x3*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_MATRIX4X4:
                    shader->SetUniformMatrix4x4(it->first, *(reinterpret_cast<Matrix4x4*>(uniform.second)));
                    break;

                case UNIFORM_TYPE_TEXTURE:
                    shader->SetUniformTexture(it->first, *(reinterpret_cast<int*>(uniform.second)));
                    break;
            }
        }

        // Prepare draw data

        // Render
	    // Get the rendering data
        unsigned int* bufferObjects;
        vector<ModelSurface_t> surfaces;
        item->mesh_->GetRenderInfo(bufferObjects, surfaces);

        // textures
        const vector<vector<Texture2D*>>* textures = item->material_->GetTextures();

        // TEMP
        // Render the mesh
        for (size_t i = 0; i < surfaces.size(); i++) {
            if (textures->size() > 0) {
                vector<Texture2D*> meshTextures = (*textures)[i];
                for (size_t j = 0; j < meshTextures.size(); j++) {
                    Texture2D* texture = meshTextures[j];
                    if (texture != nullptr) {
                        if (shader->SetUniformTexture("texture" + to_string(j), j)) {
                            texture->Bind(j);
                        }
                    } else {
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                }
            }

            glBindVertexArray(bufferObjects[i]);
                glDrawElements(GL_TRIANGLES, surfaces[i].geometry->numIndices, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Invalidate the render queue
    for (size_t i = 0; i < items_.size(); i++) {
        delete items_[i];
    }
    items_.clear();
}

}