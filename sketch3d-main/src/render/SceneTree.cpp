#include "render/SceneTree.h"

#include "render/BufferObjectManager.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "render/Renderer.h"
#include "render/RenderQueue.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include <queue>
#include <vector>
using namespace std;

// I use a monster data structure. Yes, I know, I should rethink the design but hey, I don't feel like it right now
#pragma warning(disable:4503)

namespace Sketch3D {

SceneTree::SceneTree() {
}

SceneTree::~SceneTree() {
    // Free the static batches
    StaticBatches_t::iterator it = staticBatches_.begin();
    for (; it != staticBatches_.end(); ++it) {
        TexturesToBuffersMap_t::iterator ttb_it = it->second.begin();

        for (; ttb_it != it->second.end(); ++ttb_it) {
            delete[] ttb_it->second.first.second;
        }
    }

    // Free the surfaces
    for (size_t i = 0; i < preTransformedSurfaces_.size(); i++) {
        SurfaceTriangles_t* surface = preTransformedSurfaces_[i];

        delete[] surface->vertices;
        delete[] surface->normals;
        delete[] surface->texCoords;
        delete[] surface->tangents;
        delete[] surface->indices;

        delete surface;
    }
}

void SceneTree::Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& opaqueRenderQueue, RenderQueue& transparentRenderQueue) {
    map<string, Node*>::iterator it = root_.children_.begin();
    for (; it != root_.children_.end(); ++it) {
        it->second->Render(frustumPlanes, useFrustumCulling, opaqueRenderQueue, transparentRenderQueue);
    }
}

void SceneTree::RenderStaticBatches() const {
    /*
    const Matrix4x4& viewProjectionMatrix = Renderer::GetInstance()->GetViewProjectionMatrix();
    const Matrix4x4& viewMatrix = Renderer::GetInstance()->GetViewMatrix();
    const Matrix4x4& transposedInverseViewMatrix = viewMatrix.Inverse().Transpose();

    StaticBatches_t::const_iterator it = staticBatches_.begin();
    for (; it != staticBatches_.end(); ++it) {
        TexturesToBuffersMap_t::const_iterator ttb_it = it->second.begin();
        
        // Bind the shader for the following render
        Shader* shader = it->first;
        Renderer::GetInstance()->BindShader(shader);
        shader->SetUniformMatrix4x4("viewProjection", viewProjectionMatrix);
        shader->SetUniformMatrix4x4("view", viewMatrix);
        shader->SetUniformMatrix4x4("transInvView", transposedInverseViewMatrix);

        for (; ttb_it != it->second.end(); ++ttb_it) {
            const TexturesBuffersPair_t& texturesToBuffers = ttb_it->second;

            const TexturesPair_t& texturesToBind = texturesToBuffers.first;
            const vector<BufferObject*>& bufferObjects = texturesToBuffers.second;

            // Bind the textures for the following render
            size_t numTextures = texturesToBind.first;
            Texture2D** textures = texturesToBind.second;
            for (size_t i = 0; i < numTextures; i++) {
                shader->SetUniformTexture("texture" + to_string(i), textures[i]);
            }

            // Render the actual buffer contents
            for (size_t i = 0; i < bufferObjects.size(); i++) {
                bufferObjects[i]->Render();
            }
        }
    }
    */
}

bool SceneTree::AddNode(Node* node) {
    return root_.AddChildren(node);
}

Node* SceneTree::GetNodeByName(const string& name) const {
    return root_.GetNodeByName(name);
}

bool SceneTree::RemoveNode(const Node* const node) {
    return root_.RemoveChildren(node);
}

bool SceneTree::RemoveNodeByName(const string& name) {
    return root_.RemoveChildrenByName(name);
}

void SceneTree::PerformStaticBatching() {
    //////////////////////////////////////////////////////////////////////////////////
    // WARNING: I think this function is pretty much the worst thing you'll ever see
    //////////////////////////////////////////////////////////////////////////////////

    // Get all static nodes
    vector<Node*> staticNodes;
    queue<Node*> nodes;
    nodes.push(&root_);

    while (!nodes.empty()) {
        Node* node = nodes.front();
        nodes.pop();

        if (node->IsStatic()) {
            staticNodes.push_back(node);
        }

        map<string, Node*>::iterator it = node->children_.begin();
        for (; it != node->children_.end(); ++it) {
            nodes.push(it->second);
        }
    }

    // Separate the meshes by shader and then by vertex attributes
    map<Shader*, map<size_t, vector<Node*>>> staticBatches;
    for (size_t i = 0; i < staticNodes.size(); i++) {
        Node* node = staticNodes[i];
//        Shader* shader = node->GetMaterial()->GetShader();
        Mesh* mesh = node->GetMesh();

    //    if (staticBatches.find(shader) == staticBatches.end()) {
    //        staticBatches[shader] = map<size_t, vector<Node*>>();
   //     }

      //  size_t vertexAttributes = mesh->GetVertexAttributesBitField();
     //   if (staticBatches[shader].find(vertexAttributes) == staticBatches[shader].end()) {
     //       staticBatches[shader][vertexAttributes] = vector<Node*>();
     //   }

     //   staticBatches[shader][vertexAttributes].push_back(node);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Construct the actual static batches.
    // Because there can be more than one texture per surface, we sort by textures now and
    // attribute a unique id which depends of the combination of textures used
    typedef pair<vector<float>, vector<unsigned short>> BufferObjectData_t;
    typedef pair<VertexAttributesMap_t, BufferObjectData_t> AttributesDataPair_t;
    typedef pair<size_t, AttributesDataPair_t> VertexAttributesPair_t;
    typedef map<size_t, vector<VertexAttributesPair_t>> TexturesToBufferDataMap_t;

    map<size_t, size_t> texturesId;

    // Shaders
    map<Shader*, map<size_t, vector<Node*>>>::iterator it = staticBatches.begin();
    for (; it != staticBatches.end(); ++it) {
        staticBatches_[it->first] = TexturesToBuffersMap_t();

        // Vertex attributes
        map<size_t, vector<Node*>>::iterator n_it = it->second.begin();
        for (; n_it != it->second.end(); ++n_it) {

            // Nodes
            TexturesToBufferDataMap_t texturesToBufferData;
            for (size_t i = 0; i < n_it->second.size(); i++) {
                const Matrix4x4& modelMatrix = n_it->second[i]->ConstructModelMatrix();
                const Matrix3x3& transposedInverseModelMatrix = modelMatrix.Inverse().Transpose();

                Mesh* mesh = n_it->second[i]->GetMesh();

                BufferObject** bufferObjects;
                vector<SurfaceTriangles_t*> surfaces;
                mesh->GetRenderInfo(bufferObjects, surfaces);

                // Surfaces
                for (size_t j = 0; j < surfaces.size(); j++) {
                    // Get the id for the textures
                    size_t numTextures = surfaces[j]->numTextures;
                    Texture2D** textures = surfaces[j]->textures;

                    size_t textureIdCombination = 0;
                    for (size_t k = 0; k < numTextures; k++) {
                        textureIdCombination += textures[k]->GetId() * MAX_TEXTURE_ID;
                    }

                    if (texturesId.find(textureIdCombination) == texturesId.end()) {
                        texturesId[textureIdCombination] = texturesId.size();
                    }

                    // Check if an entry for the textures already exists
                    size_t textureId = texturesId[textureIdCombination];
                    TexturesToBuffersMap_t& texturesToBuffers = staticBatches_[it->first];

                    if (texturesToBuffers.find(textureId) == texturesToBuffers.end()) {
                        Texture2D** texturesArray = nullptr;
                        
                        if (numTextures > 0) {
                            texturesArray = new Texture2D* [numTextures];
                            for (size_t k = 0; k < numTextures; k++) {
                                texturesArray[k] = textures[k];
                            }
                        }

                        TexturesPair_t texturesToBind(numTextures, texturesArray);
                        texturesToBuffers[textureId] = TexturesBuffersPair_t(texturesToBind, vector<BufferObject*>());

                        texturesToBufferData[textureId] = vector<VertexAttributesPair_t>();
                    }

                    // Append as many vertex data to the same buffer object
                    vector<VertexAttributesPair_t>& bufferData = texturesToBufferData[textureId];

                    vector<BufferObject*>& bufferObjects = texturesToBuffers[textureId].second;
                    SurfaceTriangles_t* surface = surfaces[j];
                    
                    map<size_t, VertexAttributes_t> attributesFromIndex;
                    const VertexAttributesMap_t& vertexAttributes = mesh->GetVertexAttributes();
                    VertexAttributesMap_t::const_iterator va_it = vertexAttributes.begin();

                    for (; va_it != vertexAttributes.end(); ++va_it) {
                        attributesFromIndex[va_it->second] = va_it->first;
                    }

                    vector<float> vertexData;
                    int presentVertexAttributes;
                    size_t stride;

                    //////////////////////////////////////////////////////////////////////////////////
                    // Pre-transform surface
                    //////////////////////////////////////////////////////////////////////////////////
                    SurfaceTriangles_t* transformedSurface = new SurfaceTriangles_t;
                    transformedSurface->numVertices = surface->numVertices;
                    transformedSurface->numNormals = surface->numNormals;
                    transformedSurface->numTexCoords = surface->numTexCoords;
                    transformedSurface->numTangents = surface->numTangents;

                    bool hasNormals = transformedSurface->numNormals > 0;
                    bool hasTexCoords = transformedSurface->numTexCoords > 0;
                    bool hasTangents = transformedSurface->numTangents > 0;

                    transformedSurface->vertices = new Vector3[transformedSurface->numVertices];
                    for (size_t k = 0; k < transformedSurface->numVertices; k++) {
                        transformedSurface->vertices[k] = modelMatrix * surface->vertices[k];
                    }

                    if (hasNormals) {
                        transformedSurface->normals = new Vector3[transformedSurface->numNormals];
                        for (size_t k = 0; k < transformedSurface->numNormals; k++) {
                            transformedSurface->normals[k] = (transposedInverseModelMatrix * surface->normals[k]).Normalized();
                        }
                    }

                    if (hasTexCoords) {
                        transformedSurface->texCoords = new Vector2[transformedSurface->numTexCoords];
                        for (size_t k = 0; k < transformedSurface->numTexCoords; k++) {
                            transformedSurface->texCoords[k] = surface->texCoords[k];
                        }
                    }

                    if (hasTangents) {
                        transformedSurface->tangents = new Vector3[transformedSurface->numTangents];
                        for (size_t k = 0; k < transformedSurface->numTangents; k++) {
                            transformedSurface->tangents[k] = (modelMatrix * surface->tangents[k]).Normalized();
                        }
                    }
                    preTransformedSurfaces_.push_back(transformedSurface);

                    PackSurfaceTriangleVertices(transformedSurface, attributesFromIndex, vertexData, presentVertexAttributes, stride);

                    //////////////////////////////////////////////////////////////////////////////////
                    // Append data into arrays, as much as possible in a single one
                    //////////////////////////////////////////////////////////////////////////////////
                    bool foundValidBuffer = false;
                    for (size_t k = 0; k < bufferData.size(); k++) {
                        VertexAttributesPair_t& bufferObjectData = bufferData[k];
                        AttributesDataPair_t& attributesPair = bufferObjectData.second;
                        vector<float>& bufferVertices = attributesPair.second.first;
                        vector<unsigned short>& bufferIndices = attributesPair.second.second;

                        // Buffer object must have the same vertex attributes as the node being processed
                        if (bufferObjectData.first != n_it->first) {
                            continue;
                        }

                        // There must be enough space to append the vertices
                        if ( ((bufferVertices.size() + vertexData.size()) / (stride / sizeof(float))) > 65535) {
                            continue;
                        }

                        // We have to start at the next index in the buffer
                        unsigned short startIdx = bufferVertices.size() / (stride / sizeof(float));

                        bufferVertices.reserve(bufferVertices.size() + vertexData.size());
                        for (size_t l = 0; l < vertexData.size(); l++) {
                            bufferVertices.push_back(vertexData[l]);
                        }

                        bufferIndices.reserve(bufferIndices.size() + surface->numIndices);
                        for (size_t l = 0; l < surface->numIndices; l++) {
                            bufferIndices.push_back(startIdx + surface->indices[l]);
                        }
                        
                        foundValidBuffer = true;
                        break;
                    }

                    if (!foundValidBuffer) {
                        vector<float> newVertexData;
                        vector<unsigned short> newIndexData;
                        newVertexData.reserve(vertexData.size());
                        newIndexData.reserve(surface->numIndices);

                        for (size_t l = 0; l < vertexData.size(); l++) {
                            newVertexData.push_back(vertexData[l]);
                        }

                        for (size_t l = 0; l < surface->numIndices; l++) {
                            newIndexData.push_back(surface->indices[l]);
                        }

                        BufferObjectData_t bufferObjectData(newVertexData, newIndexData);
                        AttributesDataPair_t attributesPair(vertexAttributes, bufferObjectData);
                        bufferData.push_back( VertexAttributesPair_t(presentVertexAttributes, attributesPair) );
                    }
                }
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Actually create the buffer object
            //////////////////////////////////////////////////////////////////////////////////
            TexturesToBufferDataMap_t::iterator ttb_it = texturesToBufferData.begin();
            for (; ttb_it != texturesToBufferData.end(); ++ttb_it) {
                size_t textureId = ttb_it->first;
                vector<VertexAttributesPair_t>& bufferObjectData = ttb_it->second;

                TexturesToBuffersMap_t& texturesToBuffers = staticBatches_[it->first];
                TexturesBuffersPair_t& texturesBuffersPair = texturesToBuffers[textureId];
                vector<BufferObject*>& bufferObjects = texturesBuffersPair.second;

                for (size_t i = 0; i < bufferObjectData.size(); i++) {
                    size_t presentVertexAttributes = bufferObjectData[i].first;
                    AttributesDataPair_t& attributesPair = bufferObjectData[i].second;

                    VertexAttributesMap_t& vertexAttributes = attributesPair.first;
                    BufferObjectData_t& bufferData = attributesPair.second;

                    vector<float>& vertexData = bufferData.first;
                    vector<unsigned short>& indexData = bufferData.second;

                    BufferObject* bufferObject = Renderer::GetInstance()->GetBufferObjectManager()->CreateBufferObject(vertexAttributes);
                    bufferObject->SetVertexData(vertexData, presentVertexAttributes);
                    bufferObject->SetIndexData(&indexData[0], indexData.size());
                    bufferObjects.push_back(bufferObject);
                }
            }
        }
    }
}

}