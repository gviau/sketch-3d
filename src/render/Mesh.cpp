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
                ModelSurface_t& model = surfaces_[i];
                delete[] model.geometry->vertices;
                delete[] model.geometry->normals;
                delete[] model.geometry->texCoords;
                delete[] model.geometry->tangents;
                delete[] model.geometry->bones;
                delete[] model.geometry->weights;
                delete[] model.geometry->indices;

                for (size_t j = 0; j < model.geometry->numTextures; j++) {
                    if (TextureManager::GetInstance()->CheckIfTextureLoaded(model.geometry->textures[j]->GetFilename())) {
                        TextureManager::GetInstance()->RemoveReferenceFromCache(model.geometry->textures[j]->GetFilename());
                    }
                }
                delete[] model.geometry->textures;
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
                const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                
                // Get all the textures referenced by the material
                surface->numTextures = material->GetTextureCount(aiTextureType_DIFFUSE) +
                                       material->GetTextureCount(aiTextureType_NORMALS) +
                                       material->GetTextureCount(aiTextureType_SPECULAR);
                surface->textures = new Texture2D* [surface->numTextures];

                size_t textureIdx = 0;
                for (size_t j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++) {
                    aiString textureName;
                    material->GetTexture(aiTextureType_DIFFUSE, j, &textureName);

                    Texture2D* texture = Renderer::GetInstance()->CreateTexture2DFromFile(meshPath + textureName.C_Str(), true);
                    surface->textures[textureIdx++] = texture;
                }

                for (size_t j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); j++) {
                    aiString textureName;
                    material->GetTexture(aiTextureType_NORMALS, j, &textureName);

                    Texture2D* texture = Renderer::GetInstance()->CreateTexture2DFromFile(meshPath + textureName.C_Str(), true);
                    surface->textures[textureIdx++] = texture;
                }

                for (size_t j = 0; j < material->GetTextureCount(aiTextureType_SPECULAR); j++) {
                    aiString textureName;
                    material->GetTexture(aiTextureType_SPECULAR, j, &textureName);

                    Texture2D* texture = Renderer::GetInstance()->CreateTexture2DFromFile(meshPath + textureName.C_Str(), true);
                    surface->textures[textureIdx++] = texture;
                }
            }

            ModelSurface_t modelSurface;
            modelSurface.geometry = surface;

            surfaces_.push_back(modelSurface);
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

void Mesh::AddSurface(const ModelSurface_t& surface) {
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

	    // Interleave the data
	    vector<float> data;
        data.reserve(surfaces_[i].geometry->numVertices * 3 +
                     surfaces_[i].geometry->numNormals * 3 +
                     surfaces_[i].geometry->numTexCoords * 2 +
                     surfaces_[i].geometry->numTangents * 3);

        bool hasNormals = surfaces_[i].geometry->numNormals > 0;
        bool hasTexCoords = surfaces_[i].geometry->numTexCoords > 0;
        bool hasTangents = surfaces_[i].geometry->numTangents > 0;
        bool hasBones = surfaces_[i].geometry->numBones > 0;
        bool hasWeights = surfaces_[i].geometry->numWeights > 0;

        Vector3 vertex;
        for (size_t j = 0; j < surfaces_[i].geometry->numVertices; j++) {
            map<size_t, VertexAttributes_t>::iterator v_it = attributesFromIndex.begin();

            for (; v_it != attributesFromIndex.end(); ++v_it) {
                switch (v_it->second) {
                    case VERTEX_ATTRIBUTES_POSITION:
                        vertex = surfaces_[i].geometry->vertices[j];
                        data.push_back(vertex.x); data.push_back(vertex.y); data.push_back(vertex.z);
                        break;

                    case VERTEX_ATTRIBUTES_NORMAL:
                        if (hasNormals) {
                            Vector3 normal = surfaces_[i].geometry->normals[j];
                            data.push_back(normal.x); data.push_back(normal.y); data.push_back(normal.z);
                        }
                        break;

                    case VERTEX_ATTRIBUTES_TEX_COORDS:
                        if (hasTexCoords) {
                            Vector2 texCoords = surfaces_[i].geometry->texCoords[j];
                            data.push_back(texCoords.x); data.push_back(texCoords.y);
                        }
                        break;

                    case VERTEX_ATTRIBUTES_TANGENT:
                        if (hasTangents) {
                            Vector3 tangents = surfaces_[i].geometry->tangents[j];
                            data.push_back(tangents.x); data.push_back(tangents.y); data.push_back(tangents.z);
                        }
                        break;
                }
            }
	    }

        int presentVertexAttributes = 0;
        if (hasNormals) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_NORMAL;
        }

        if (hasTexCoords) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_TEX_COORDS;
        }

        if (hasTangents) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_TANGENT;
        }

        if (!bufferObject->SetVertexData(data, presentVertexAttributes)) {
            Logger::GetInstance()->Error("The vertex attributes are not all present");
            FreeMeshMemory();
            break;
        }

        bufferObject->SetIndexData(surfaces_[i].geometry->indices, surfaces_[i].geometry->numIndices);
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
        data.reserve(surfaces_[i].geometry->numVertices * 3 +
                     surfaces_[i].geometry->numNormals * 3 +
                     surfaces_[i].geometry->numTexCoords * 2 +
                     surfaces_[i].geometry->numTangents * 3);

        bool hasNormals = surfaces_[i].geometry->numNormals > 0;
        bool hasTexCoords = surfaces_[i].geometry->numTexCoords > 0;
        bool hasTangents = surfaces_[i].geometry->numTangents > 0;

        // Interleave the data
        Vector3 vertex;
        for (size_t j = 0; j < surfaces_[i].geometry->numVertices; j++) {
            map<size_t, VertexAttributes_t>::iterator v_it = attributesFromIndex.begin();

            for (; v_it != attributesFromIndex.end(); ++v_it) {
                switch (v_it->second) {
                    case VERTEX_ATTRIBUTES_POSITION:
                        vertex = surfaces_[i].geometry->vertices[j];
                        data.push_back(vertex.x); data.push_back(vertex.y); data.push_back(vertex.z);
                        break;

                    case VERTEX_ATTRIBUTES_NORMAL:
                        if (hasNormals) {
                            Vector3 normal = surfaces_[i].geometry->normals[j];
                            data.push_back(normal.x); data.push_back(normal.y); data.push_back(normal.z);
                        }
                        break;

                    case VERTEX_ATTRIBUTES_TEX_COORDS:
                        if (hasTexCoords) {
                            Vector2 texCoords = surfaces_[i].geometry->texCoords[j];
                            data.push_back(texCoords.x); data.push_back(texCoords.y);
                        }
                        break;

                    case VERTEX_ATTRIBUTES_TANGENT:
                        if (hasTangents) {
                            Vector3 tangents = surfaces_[i].geometry->tangents[j];
                            data.push_back(tangents.x); data.push_back(tangents.y); data.push_back(tangents.z);
                        }
                        break;
                }
            }
	    }

        int presentVertexAttributes = 0;
        if (hasNormals) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_NORMAL;
        }

        if (hasTexCoords) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_TEX_COORDS;
        }

        if (hasTangents) {
            presentVertexAttributes |= VERTEX_ATTRIBUTES_TANGENT;
        }

        bufferObjects_[i]->SetVertexData(data, presentVertexAttributes);
    }
}

void Mesh::GetRenderInfo(BufferObject**& bufferObjects, vector<ModelSurface_t>& surfaces) const {
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
                ModelSurface_t& model = surfaces_[i];
                delete[] model.geometry->vertices;
                delete[] model.geometry->normals;
                delete[] model.geometry->texCoords;
                delete[] model.geometry->tangents;
                delete[] model.geometry->bones;
                delete[] model.geometry->weights;
                delete[] model.geometry->indices;

                for (size_t j = 0; j < model.geometry->numTextures; j++) {
                    Texture2D* texture = model.geometry->textures[j];

                    if (texture != nullptr) {
                        if (TextureManager::GetInstance()->CheckIfTextureLoaded(texture->GetFilename())) {
                            TextureManager::GetInstance()->RemoveReferenceFromCache(texture->GetFilename());
                        }
                    }
                }
                delete[] model.geometry->textures;
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
    Vector3 meanPosition;
    size_t numberOfVertices = 0;

    Vector3 minX;
    Vector3 minY;
    Vector3 minZ;
    Vector3 maxX;
    Vector3 maxY;
    Vector3 maxZ;

    for (size_t i = 0; i < surfaces_.size(); i++) {
        SurfaceTriangles_t* surface = surfaces_[i].geometry;

        numberOfVertices += surface->numVertices;
        for (size_t j = 0; j < surface->numVertices; j++) {
            meanPosition += surface->vertices[j];
            float x = surface->vertices[j].x;
            float y = surface->vertices[j].y;
            float z = surface->vertices[j].z;

            if (x < minX.x) { minX.x = x; minX.y = y; minX.z = z; } else if (x > maxX.x) { maxX.x = x; maxX.y = y; maxX.z = z; }
            if (y < minY.y) { minY.x = x; minY.y = y; minY.z = z; } else if (y > maxY.y) { maxY.x = x; maxY.y = y; maxY.z = z; }
            if (z < minZ.z) { minZ.x = x; minZ.y = y; minZ.z = z; } else if (z > maxZ.z) { maxZ.x = x; maxZ.y = y; maxZ.z = z; }
        }
    }

    meanPosition /= (float)numberOfVertices;

    float magnitude = 0.0f;
    float maxMagnitude = 0.0f;
    magnitude = minX.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }
    magnitude = minY.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }
    magnitude = minZ.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }

    magnitude = maxX.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }
    magnitude = maxY.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }
    magnitude = maxZ.Dot(minX); if (magnitude > maxMagnitude) { maxMagnitude = magnitude; }

    boundingSphere_.SetCenter(meanPosition);
    boundingSphere_.SetRadius(sqrtf(maxMagnitude));
}

}
