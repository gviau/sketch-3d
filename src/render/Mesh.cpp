#include "render/Mesh.h"

#include "render/ModelManager.h"
#include "render/Renderer.h"
#include "render/Texture2D.h"
#include "render/TextureManager.h"
#include "system/Logger.h"
#include "system/Utils.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <FreeImage.h>

#include <queue>

namespace Sketch3D {

#define LOADED_MESH_FROM_FILE           0x01
#define MESH_USES_NORMALS               0x02
#define MESH_USES_TEXTURE_COORDINATES   0x04
#define MESH_USES_USES_TANGENTS         0x08

Mesh::Mesh() : meshType_(MESH_TYPE_STATIC), filename_(""), fromCache_(false), importer_(nullptr), vbo_(nullptr), ibo_(nullptr), vao_(nullptr)
{
}

Mesh::Mesh(const string& filename, bool useNormals, bool useTextureCoordinates, bool useTangents, MeshType_t meshType) : meshType_(MESH_TYPE_STATIC),
        filename_(""), fromCache_(false), importer_(nullptr), vbo_(nullptr), ibo_(nullptr), vao_(nullptr), loadedProperties_(0)
{
    Load(filename, useNormals, useTextureCoordinates, useTangents, meshType);
    Initialize(meshType);
}

Mesh::Mesh(const Mesh& src) : meshType_(src.meshType_), filename_(src.filename_), fromCache_(false), importer_(nullptr),
        vbo_(nullptr), ibo_(nullptr), vao_(nullptr), loadedProperties_(src.loadedProperties_)
{
    if (((loadedProperties_ & LOADED_MESH_FROM_FILE) != 0) && ModelManager::GetInstance()->CheckIfModelLoaded(filename_)) {
        bool useNormals = ((loadedProperties_ & MESH_USES_NORMALS) != 0);
        bool useTextureCoordinates = ((loadedProperties_ & MESH_USES_TEXTURE_COORDINATES) != 0);
        bool useTangents = ((loadedProperties_ & MESH_USES_USES_TANGENTS) != 0);

        Load(filename_, useNormals, useTextureCoordinates, useTangents, meshType_);
        Initialize(meshType_);
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
        vbo_ = nullptr;
        ibo_ = nullptr;
        vao_ = nullptr;
        loadedProperties_ = rhs.loadedProperties_;

        if (((loadedProperties_ & LOADED_MESH_FROM_FILE) != 0) && ModelManager::GetInstance()->CheckIfModelLoaded(filename_)) {
            bool useNormals = ((loadedProperties_ & MESH_USES_NORMALS) != 0);
            bool useTextureCoordinates = ((loadedProperties_ & MESH_USES_TEXTURE_COORDINATES) != 0);
            bool useTangents = ((loadedProperties_ & MESH_USES_USES_TANGENTS) != 0);

            Load(filename_, useNormals, useTextureCoordinates, useTangents, meshType_);
            Initialize(meshType_);
        }
    }

    return *this;
}

void Mesh::Load(const string& filename, bool useNormals, bool useTextureCoordinates, bool useTangents, MeshType_t meshType) {
    if (filename == filename_) {
        return;
    }

    // Delete last model if present
    if (surfaces_.size() != 0) {
        if (fromCache_) {
            ModelManager::GetInstance()->RemoveReferenceFromCache(filename_);
        } else {
            for (size_t i = 0; i < surfaces_.size(); i++) {
                ModelSurface_t& model = surfaces_[i];
                delete[] model.geometry->vertices;
                delete[] model.geometry->normals;
                delete[] model.geometry->texCoords;
                delete[] model.geometry->tangents;
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
        surfaces_ = ModelManager::GetInstance()->LoadFromCache(filename);
        filename_ = filename;
        Initialize(meshType);
        fromCache_ = true;
        return;
    }

    // Load if not present in cache and cache it for future loads
    delete importer_;
    importer_ = new Assimp::Importer;
    unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType;
    if (useNormals) {
        flags |= aiProcess_GenNormals | aiProcess_FixInfacingNormals;
    }

    if (useTextureCoordinates) {
        flags |= aiProcess_GenUVCoords;
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
                surface->numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
                surface->textures = new Texture2D* [surface->numTextures];

                for (size_t j = 0; j < surface->numTextures; j++) {
                    aiString textureName;
                    material->GetTexture(aiTextureType_DIFFUSE, j, &textureName);

                    Texture2D* texture = Renderer::GetInstance()->CreateTexture2DFromFile(meshPath + textureName.C_Str());
                    surface->textures[j] = texture;
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
    Initialize(meshType);
    fromCache_ = true;

    loadedProperties_ = LOADED_MESH_FROM_FILE;
    if (useNormals) {
        loadedProperties_ |= MESH_USES_NORMALS;
    }

    if (useTextureCoordinates) {
        loadedProperties_ |= MESH_USES_TEXTURE_COORDINATES;
    }

    if (useTangents) {
        loadedProperties_ |= MESH_USES_USES_TANGENTS;
    }

    Logger::GetInstance()->Info("Successfully loaded mesh from file " + filename);
}

void Mesh::AddSurface(const ModelSurface_t& surface) {
    surfaces_.push_back(surface);
}

void Mesh::Initialize(MeshType_t meshType) {
    meshType_ = meshType;

	// TEMP - construct the buffers
    vbo_ = new unsigned int[surfaces_.size()];
    ibo_ = new unsigned int[surfaces_.size()];
    vao_ = new unsigned int[surfaces_.size()];

    glGenBuffers(surfaces_.size(), vbo_);
    glGenBuffers(surfaces_.size(), ibo_);
    glGenVertexArrays(surfaces_.size(), vao_);

    for (size_t i = 0; i < surfaces_.size(); i++) {
	    // Interleave the data
	    vector<float> data;
        data.reserve(surfaces_[i].geometry->numVertices * 3 +
                     surfaces_[i].geometry->numNormals * 3 +
                     surfaces_[i].geometry->numTexCoords * 2 +
                     surfaces_[i].geometry->numTangents * 3);

        bool hasVertices = surfaces_[i].geometry->numVertices > 0;
        bool hasNormals = surfaces_[i].geometry->numNormals > 0;
        bool hasTexCoords = surfaces_[i].geometry->numTexCoords > 0;
        bool hasTangents = surfaces_[i].geometry->numTangents > 0;

        for (size_t j = 0; j < surfaces_[i].geometry->numVertices; j++) {
            if (hasVertices) {
                Vector3 vertex = surfaces_[i].geometry->vertices[j];
                data.push_back(vertex.x); data.push_back(vertex.y); data.push_back(vertex.z);
            }

            if (hasNormals) {
                Vector3 normal = surfaces_[i].geometry->normals[j];
                data.push_back(normal.x); data.push_back(normal.y); data.push_back(normal.z);
            }

            if (hasTexCoords) {
                Vector2 texCoords = surfaces_[i].geometry->texCoords[j];
                data.push_back(texCoords.x); data.push_back(texCoords.y);
            }

            if (hasTangents) {
                Vector3 tangents = surfaces_[i].geometry->tangents[j];
                data.push_back(tangents.x); data.push_back(tangents.y); data.push_back(tangents.z);
            }
	    }

        size_t stride = ((hasVertices) ? sizeof(Vector3) : 0) +
                        ((hasNormals) ? sizeof(Vector3) : 0) +
                        ((hasTexCoords) ? sizeof(Vector2) : 0) +
                        ((hasTangents) ? sizeof(Vector3) : 0);
        size_t normalsOffset = (hasNormals) ? sizeof(Vector3) : 0;
        size_t texCoordsOffset = (hasTexCoords) ? (hasNormals) ? sizeof(Vector3) + sizeof(Vector3) : sizeof(Vector3) : 0;

        size_t tangentsOffset = 0;
        if (hasTangents) {
            if (hasTexCoords) {
                if (hasNormals) {
                    tangentsOffset = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
                } else {
                    tangentsOffset = sizeof(Vector3) + sizeof(Vector2);
                }
            } else {
                if (hasNormals) {
                    tangentsOffset = sizeof(Vector3) + sizeof(Vector3);
                } else {
                    tangentsOffset = sizeof(Vector3);
                }
            }
        }

        size_t normalsArrayIndex = (hasNormals) ? 1 : 0;
        size_t texCoordsArrayIndex = (hasTexCoords) ? (hasNormals) ? 2 : 1 : 0;

        size_t tangentsArrayIndex = 0;
        if (hasTangents) {
            if (hasTexCoords) {
                if (hasNormals) {
                    tangentsArrayIndex = 3;
                } else {
                    tangentsArrayIndex = 2;
                }
            } else {
                if (hasNormals) {
                    tangentsArrayIndex = 2;
                } else {
                    tangentsArrayIndex = 1;
                }
            }
        }

        // We first create the vertex array object and then bind the vertex and index buffer objects
	    glBindVertexArray(vao_[i]);

        // Vertex buffer object
        int type = (meshType_ == MESH_TYPE_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
	    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], type);


        glEnableVertexAttribArray(0);
	    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, stride, 0);

        if (hasNormals) {
            glEnableVertexAttribArray(normalsArrayIndex);
            glVertexAttribPointer((GLuint)normalsArrayIndex, 3, GL_FLOAT, GL_FALSE, stride, (void*)normalsOffset);
        }

        if (hasTexCoords) {
            glEnableVertexAttribArray(texCoordsArrayIndex);
            glVertexAttribPointer((GLuint)texCoordsArrayIndex, 2, GL_FLOAT, GL_FALSE, stride, (void*)texCoordsOffset);
        }

        if (hasTangents) {
            glEnableVertexAttribArray(tangentsArrayIndex);
            glVertexAttribPointer((GLuint)tangentsArrayIndex, 3, GL_FLOAT, GL_FALSE, stride, (void*)tangentsOffset);
        }

        // Index buffer object
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaces_[i].geometry->numIndices * sizeof(unsigned short), surfaces_[i].geometry->indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Mesh::UpdateMeshData() const {
    if (meshType_ == MESH_TYPE_STATIC) {
        return;
    }

    for (size_t i = 0; i < surfaces_.size(); i++) {
	    vector<float> data;
        data.reserve(surfaces_[i].geometry->numVertices * 3 +
                     surfaces_[i].geometry->numNormals * 3 +
                     surfaces_[i].geometry->numTexCoords * 2 +
                     surfaces_[i].geometry->numTangents * 3);

        bool hasVertices = surfaces_[i].geometry->numVertices > 0;
        bool hasNormals = surfaces_[i].geometry->numNormals > 0;
        bool hasTexCoords = surfaces_[i].geometry->numTexCoords > 0;
        bool hasTangents = surfaces_[i].geometry->numTangents > 0;

        for (size_t j = 0; j < surfaces_[i].geometry->numVertices; j++) {
            if (hasVertices) {
                Vector3 vertex = surfaces_[i].geometry->vertices[j];
                data.push_back(vertex.x); data.push_back(vertex.y); data.push_back(vertex.z);
            }

            if (hasNormals) {
                Vector3 normal = surfaces_[i].geometry->normals[j];
                data.push_back(normal.x); data.push_back(normal.y); data.push_back(normal.z);
            }

            if (hasTexCoords) {
                Vector2 texCoords = surfaces_[i].geometry->texCoords[j];
                data.push_back(texCoords.x); data.push_back(texCoords.y);
            }

            if (hasTangents) {
                Vector3 tangents = surfaces_[i].geometry->tangents[j];
                data.push_back(tangents.x); data.push_back(tangents.y); data.push_back(tangents.z);
            }
	    }

        int type = (meshType_ == MESH_TYPE_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), &data[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Mesh::GetRenderInfo(unsigned int*& bufferObjects, vector<ModelSurface_t>& surfaces) const {
    bufferObjects = vao_;
    surfaces = surfaces_;
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
            ModelManager::GetInstance()->RemoveReferenceFromCache(filename_);
        }

        glDeleteBuffers(surfaces_.size(), vbo_);
        glDeleteBuffers(surfaces_.size(), ibo_);
        glDeleteVertexArrays(surfaces_.size(), vao_);

        delete[] vbo_;
        delete[] ibo_;
        delete[] vao_;
    }
}

}