#include "render/Mesh.h"

#include "math/Sphere.h"

#include "render/BufferObject.h"
#include "render/BufferObjectManager.h"
#include "render/ModelManager.h"
#include "render/Renderer.h"
#include "render/Texture2D.h"
#include "render/TextureManager.h"

#include "system/Logger.h"
#include "system/Utils.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <FreeImage.h>

#include <memory>
#include <queue>

namespace Sketch3D {

Mesh::Mesh(MeshType_t meshType) : meshType_(meshType), filename_(""), fromCache_(false), importer_(nullptr), bufferObjects_(nullptr)
{
}

Mesh::Mesh(const string& filename, const VertexAttributesMap_t& vertexAttributes, MeshType_t meshType, bool counterClockWise) : meshType_(meshType),
        filename_(""), fromCache_(false), importer_(nullptr), bufferObjects_(nullptr)
{
    Load(filename, vertexAttributes, counterClockWise);
    Initialize(vertexAttributes);
}

Mesh::Mesh(const Mesh& src) : meshType_(src.meshType_), filename_(src.filename_), fromCache_(false), importer_(nullptr),
        bufferObjects_(nullptr)
{
    if (ModelManager::GetInstance()->CheckIfModelLoaded(filename_)) {
        Load(filename_, src.vertexAttributes_);
        Initialize(src.vertexAttributes_);
    }
}

Mesh::~Mesh() {
    FreeMeshMemory();
}

Mesh& Mesh::operator= (const Mesh& rhs) {
    if (this != &rhs) {
        FreeMeshMemory();

        meshType_ = rhs.meshType_;
        filename_ = rhs.filename_;
        fromCache_ = false;
        importer_ = nullptr;
        bufferObjects_ = nullptr;

        if (ModelManager::GetInstance()->CheckIfModelLoaded(filename_)) {
            Load(filename_, rhs.vertexAttributes_);
            Initialize(rhs.vertexAttributes_);
        }
    }

    return *this;
}

void Mesh::Load(const string& filename, const VertexAttributesMap_t& vertexAttributes, bool counterClockWise) {
    if (filename == filename_) {
        return;
    }

    // Delete last model if present
    if (surfaces_.size() != 0) {
        if (fromCache_) {
            ModelManager::GetInstance()->RemoveModelReferenceFromCache(filename_);
        } else {
            for (size_t i = 0; i < surfaces_.size(); i++) {
                SurfaceTriangles_t* surface = surfaces_[i];

                delete[] surface->vertices;
                delete[] surface->normals;
                delete[] surface->texCoords;
                delete[] surface->tangents;
                delete[] surface->bones;
                delete[] surface->weights;
                delete[] surface->indices;

                for (size_t j = 0; j < surface->numTextures; j++) {
                    if (TextureManager::GetInstance()->CheckIfTextureLoaded(surface->textures[j]->GetFilename())) {
                        TextureManager::GetInstance()->RemoveTextureReferenceFromCache(surface->textures[j]->GetFilename());
                    }
                }
                delete[] surface->textures;
            }
        }
    }

    // Check cache first
    if (ModelManager::GetInstance()->CheckIfModelLoaded(filename)) {
        surfaces_ = ModelManager::GetInstance()->LoadModelFromCache(filename);
        filename_ = filename;
        Initialize(vertexAttributes);
        fromCache_ = true;
        return;
    }

    // Determine what does the mesh uses
    bool useNormals = vertexAttributes.find(VERTEX_ATTRIBUTES_NORMAL) != vertexAttributes.end();
    bool useTextureCoordinates = vertexAttributes.find(VERTEX_ATTRIBUTES_TEX_COORDS) != vertexAttributes.end();
    bool useTangents = vertexAttributes.find(VERTEX_ATTRIBUTES_TANGENT) != vertexAttributes.end();

    // Load if not present in cache and cache it for future loads
    delete importer_;
    importer_ = new Assimp::Importer;
    unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType;
    if (useNormals) {
        flags |= aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals;
    }

    if (useTextureCoordinates) {
        flags |= aiProcess_GenUVCoords;
    }

    if (!counterClockWise) {
        flags |= aiProcess_FlipWindingOrder;
    }

    const aiScene* scene = importer_->ReadFile(filename, flags);
    if (scene == nullptr) {
        Logger::GetInstance()->Error("Couldn't load mesh " + filename);
        delete importer_;
        importer_ = nullptr;
        return;
    }

    if (useTangents) {
        importer_->ApplyPostProcessing(aiProcess_CalcTangentSpace);
    }

    // Retrive the path from which the mesh was loaded
    vector<string> tokens = Tokenize(filename, '/');
    string meshPath = "";
    if (tokens.size() > 0 ) {
        for (size_t i = 0; i < tokens.size() - 1; i++) {
            meshPath += tokens[i] + "/";
        }
    }

    set<size_t> textureSet;
    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);

    while (!nodes.empty()) {
        const aiNode* node = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < node->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            SurfaceTriangles_t* surface = new SurfaceTriangles_t;
            size_t numVertices = mesh->mNumVertices;

            if (mesh->HasPositions()) {
                surface->numVertices = numVertices;
                surface->vertices = new Vector3[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& vertex = mesh->mVertices[j];
                    surface->vertices[j].x = vertex.x;
                    surface->vertices[j].y = vertex.y;
                    surface->vertices[j].z = vertex.z;
                }
            }

            if (useNormals && mesh->HasNormals()) {
                surface->numNormals = numVertices;
                surface->normals = new Vector3[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& normal = mesh->mNormals[j];
                    surface->normals[j].x = normal.x;
                    surface->normals[j].y = normal.y;
                    surface->normals[j].z = normal.z;
                }
            }

            if (useTextureCoordinates && mesh->HasTextureCoords(0)) {
                surface->numTexCoords = numVertices;
                surface->texCoords = new Vector2[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& texCoord = mesh->mTextureCoords[0][j];
                    surface->texCoords[j].x = texCoord.x;
                    surface->texCoords[j].y = texCoord.y;
                }
            }

            if (useTangents && mesh->HasTangentsAndBitangents()) {
                surface->numTangents = numVertices;
                surface->tangents = new Vector3[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& tangent = mesh->mTangents[j];
                    surface->tangents[j].x = tangent.x;
                    surface->tangents[j].y = tangent.y;
                    surface->tangents[j].z = tangent.z;
                }
            }

            if (mesh->HasFaces()) {
                surface->numIndices = mesh->mNumFaces * 3;
                surface->indices = new unsigned short[surface->numIndices];
                size_t idx = 0;

                for (size_t j = 0; j < mesh->mNumFaces; j++) {
                    const aiFace& face = mesh->mFaces[j];

                    for (size_t k = 0; k < face.mNumIndices; k++) {
                        surface->indices[idx++] = face.mIndices[k];
                    }
                }
            }

            if (scene->HasMaterials()) {
                set<size_t>::iterator it = textureSet.find(mesh->mMaterialIndex);
                if (it == textureSet.end()) {
                    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                
                    // Get all the textures referenced by the material
                    surface->numTextures = material->GetTextureCount(aiTextureType_DIFFUSE) +
                                           material->GetTextureCount(aiTextureType_NORMALS) +
                                           material->GetTextureCount(aiTextureType_SPECULAR);

                    // It seems that sometime the normal map may be stored in the height map
                    if (material->GetTextureCount(aiTextureType_NORMALS) == 0) {
                        surface->numTextures += material->GetTextureCount(aiTextureType_HEIGHT);
                    }

                    surface->textures = new Texture2D* [surface->numTextures];
                    vector<string> texturesFilename;

                    size_t textureIdx = 0;
                    aiTextureType textureTypes[] = { aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR };
                    size_t numTextureTypes = sizeof(textureTypes) / sizeof(aiTextureType);

                    for (size_t type = 0; type < numTextureTypes; type++) {
                        aiTextureType textureType = textureTypes[type];
                        if (textureType == aiTextureType_NORMALS && material->GetTextureCount(textureType) == 0) {
                            textureType = aiTextureType_HEIGHT;
                        }

                        for (size_t j = 0; j < material->GetTextureCount(textureType); j++) {
                            aiString textureName;
                            material->GetTexture(textureType, j, &textureName);

                            Texture2D* texture = Renderer::GetInstance()->CreateTexture2DFromFile(meshPath + textureName.C_Str(), true);
                            surface->textures[textureIdx++] = texture;

                            texturesFilename.push_back(textureName.C_Str());
                        }
                    }

                    textureSet.insert(mesh->mMaterialIndex);
                    TextureManager::GetInstance()->CacheTextureSet(texturesFilename, surface->textures);
                } else {
                    vector<string> texturesFilename;
                    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                
                    // Get all the textures referenced by the material
                    surface->numTextures = material->GetTextureCount(aiTextureType_DIFFUSE) +
                                           material->GetTextureCount(aiTextureType_NORMALS) +
                                           material->GetTextureCount(aiTextureType_SPECULAR);

                    // It seems that sometime the normal map may be stored in the height map
                    if (material->GetTextureCount(aiTextureType_NORMALS) == 0) {
                        surface->numTextures += material->GetTextureCount(aiTextureType_HEIGHT);
                    }

                    size_t textureIdx = 0;
                    aiTextureType textureTypes[] = { aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR };
                    size_t numTextureTypes = sizeof(textureTypes) / sizeof(aiTextureType);

                    for (size_t type = 0; type < numTextureTypes; type++) {
                        aiTextureType textureType = textureTypes[type];
                        if (textureType == aiTextureType_NORMALS && material->GetTextureCount(textureType) == 0) {
                            textureType = aiTextureType_HEIGHT;
                        }

                        for (size_t j = 0; j < material->GetTextureCount(textureType); j++) {
                            aiString textureName;
                            material->GetTexture(textureType, j, &textureName);
                            texturesFilename.push_back(textureName.C_Str());
                        }
                    }

                    surface->textures = TextureManager::GetInstance()->LoadTextureSetFromCache(texturesFilename);
                }
            }

            surfaces_.push_back(surface);
        }

        for (size_t i = 0; i < node->mNumChildren; i++) {
            nodes.push(node->mChildren[i]);
        }
    }

    // Cache the model for future loads
    ModelManager::GetInstance()->CacheModel(filename, surfaces_);
    filename_ = filename;
    fromCache_ = true;

    Logger::GetInstance()->Info("Successfully loaded mesh from file " + filename);
}

void Mesh::AddSurface(SurfaceTriangles_t* surface) {
    surfaces_.push_back(surface);
}

void Mesh::Initialize(const VertexAttributesMap_t& vertexAttributes) {
    vertexAttributes_ = vertexAttributes;

    // Calculate offset and array index depending on vertex attributes provided by the user
    map<size_t, VertexAttributes_t> attributesFromIndex;
    VertexAttributesMap_t::iterator it = vertexAttributes_.begin();
    for (; it != vertexAttributes_.end(); ++it) {
        attributesFromIndex[it->second] = it->first;
    }

    bufferObjects_ = new BufferObject* [surfaces_.size()];
    BufferUsage_t bufferUsage = (meshType_ == MESH_TYPE_STATIC) ? BUFFER_USAGE_STATIC : BUFFER_USAGE_DYNAMIC;

    for (size_t i = 0; i < surfaces_.size(); i++) {
        bufferObjects_[i] = Renderer::GetInstance()->GetBufferObjectManager()->CreateBufferObject(vertexAttributes_, bufferUsage);
        BufferObject* bufferObject = bufferObjects_[i];

	    vector<float> data;
        int presentVertexAttributes;
        size_t stride;

        PackSurfaceTriangleVertices(surfaces_[i], attributesFromIndex, data, presentVertexAttributes, stride);

        if (bufferObject->SetVertexData(data, presentVertexAttributes) != BUFFER_OBJECT_ERROR_NONE) {
            Logger::GetInstance()->Error("The vertex attributes are not all present");
            FreeMeshMemory();
            break;
        }

        bufferObject->SetIndexData(surfaces_[i]->indices, surfaces_[i]->numIndices);
    }

    ConstructBoundingSphere();
}

void Mesh::UpdateMeshData() const {
    if (meshType_ == MESH_TYPE_STATIC) {
        return;
    }

    map<size_t, VertexAttributes_t> attributesFromIndex;
    VertexAttributesMap_t::const_iterator it = vertexAttributes_.begin();
    for (; it != vertexAttributes_.end(); ++it) {
        attributesFromIndex[it->second] = it->first;
    }

    for (size_t i = 0; i < surfaces_.size(); i++) {
	    vector<float> data;
        int presentVertexAttributes;
        size_t stride;

        PackSurfaceTriangleVertices(surfaces_[i], attributesFromIndex, data, presentVertexAttributes, stride);

        bufferObjects_[i]->SetVertexData(data, presentVertexAttributes);
    }
}

void Mesh::PrepareInstancingData() {
    for (size_t i = 0; i < surfaces_.size(); i++) {
        bufferObjects_[i]->PrepareInstanceBuffers();
    }
}

void Mesh::GetRenderInfo(BufferObject**& bufferObjects, vector<SurfaceTriangles_t*>& surfaces) const {
    bufferObjects = bufferObjects_;
    surfaces = surfaces_;
}

const Sphere& Mesh::GetBoundingSphere() const {
    return boundingSphere_;
}

void Mesh::FreeMeshMemory() {
    delete importer_;

    if (surfaces_.size() > 0) {
        if (!fromCache_) {
            for (size_t i = 0; i < surfaces_.size(); i++) {
                SurfaceTriangles_t* surface = surfaces_[i];

                delete[] surface->vertices;
                delete[] surface->normals;
                delete[] surface->texCoords;
                delete[] surface->tangents;
                delete[] surface->bones;
                delete[] surface->weights;
                delete[] surface->indices;

                for (size_t j = 0; j < surface->numTextures; j++) {
                    Texture2D* texture = surface->textures[j];

                    if (texture != nullptr) {
                        if (TextureManager::GetInstance()->CheckIfTextureLoaded(texture->GetFilename())) {
                            TextureManager::GetInstance()->RemoveTextureReferenceFromCache(texture->GetFilename());
                        }
                    }
                }
                delete[] surface->textures;
            }
        } else {
            ModelManager::GetInstance()->RemoveModelReferenceFromCache(filename_);
        }

        for (size_t i = 0; i < surfaces_.size(); i++) {
            Renderer::GetInstance()->GetBufferObjectManager()->DeleteBufferObject(bufferObjects_[i]);
        }
        delete[] bufferObjects_;

        surfaces_.clear();
    }
}

void Mesh::ConstructBoundingSphere() {
    vector<Vector3> vertices;
    size_t size = 0;
    for (size_t i = 0; i < surfaces_.size(); i++) {
        size += surfaces_[i]->numVertices;
    }
    vertices.reserve(size);

    for (size_t i = 0; i < surfaces_.size(); i++) {
        for (size_t j = 0; j < surfaces_[i]->numVertices; j++) {
            vertices.push_back(surfaces_[i]->vertices[j]);
        }
    }

    // From http://stackoverflow.com/questions/17331203/bouncing-bubble-algorithm-for-smallest-enclosing-sphere
    // Original algorithm is from Bo Tian
    Vector3 center = vertices[0];
    float radius = 0.0001f;
    Vector3 pos, diff;
    float length, alpha, alphaSq;

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < vertices.size(); j++) {
            pos = vertices[j];
            diff = pos - center;
            length = diff.Length();

            if (length > radius) {
                alpha = length / radius;
                alphaSq = alpha * alpha;
                radius = 0.5f * (alpha + 1.0f / alpha) * radius;
                center = 0.5f * ((1.0f + 1.0f / alphaSq) * center + (1.0f - 1.0f / alphaSq) * pos);
            }
        }
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        pos = vertices[i];
        diff = pos - center;
        length = diff.Length();

        if (length > radius) {
            radius = (radius + length) / 2.0f;
            center = center + ((length - radius) / length * diff);
        }
    }

    boundingSphere_.SetCenter(center);
    boundingSphere_.SetRadius(radius);
}

const VertexAttributesMap_t& Mesh::GetVertexAttributes() const {
    return vertexAttributes_;
}

size_t Mesh::GetVertexAttributesBitField() const {
    size_t vertexAttributes = 0;
    VertexAttributesMap_t::const_iterator it = vertexAttributes_.begin();
    for (; it != vertexAttributes_.end(); ++it) {
        vertexAttributes |= it->first;
    }

    return vertexAttributes;
}
}
