#include "render/Mesh.h"

#include "render/ModelManager.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <queue>

namespace Sketch3D {

Mesh::Mesh() : meshType_(MESH_TYPE_STATIC), filename_(""), fromCache_(false), importer_(nullptr), vbo_(nullptr), ibo_(nullptr), vao_(nullptr)
{
}

Mesh::Mesh(const string& filename, MeshType_t meshType) : meshType_(MESH_TYPE_STATIC), filename_(""), fromCache_(false), importer_(nullptr),
        vbo_(nullptr), ibo_(nullptr), vao_(nullptr)
{
    Load(filename, meshType);
    Initialize(meshType);
}

Mesh::~Mesh() {
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
            }
        }

        glDeleteBuffers(surfaces_.size(), vbo_);
        glDeleteBuffers(surfaces_.size(), ibo_);
        glDeleteVertexArrays(surfaces_.size(), vao_);

        delete[] vbo_;
        delete[] ibo_;
        delete[] vao_;
    }
}

void Mesh::Load(const string& filename, MeshType_t meshType) {
    if (filename == filename_) {
        return;
    }

    // Delete last model if present
    if (surfaces_.size() != 0) {
        if (fromCache_) {
            ModelManager::GetInstance()->RemoveReferenceFromCache(filename_);
            fromCache_ = false;
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
    importer_ = new Assimp::Importer;
    const aiScene* scene = importer_->ReadFile(filename, aiProcessPreset_TargetRealtime_Fast);

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

            if (mesh->HasNormals()) {
                surface->numNormals = numVertices;
                surface->normals = new Vector3[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& normal = mesh->mNormals[j];
                    surface->normals[j].x = normal.x;
                    surface->normals[j].y = normal.y;
                    surface->normals[j].z = normal.z;
                }
            }

            if (mesh->HasTextureCoords(0)) {
                surface->numTexCoords = numVertices;
                surface->texCoords = new Vector2[numVertices];

                for (size_t j = 0; j < numVertices; j++) {
                    const aiVector3D& texCoord = mesh->mTextureCoords[0][j];
                    surface->texCoords[j].x = texCoord.x;
                    surface->texCoords[j].y = texCoord.y;
                }
            }

            if (mesh->HasTangentsAndBitangents()) {
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

}