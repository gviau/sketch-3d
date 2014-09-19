#include "render/Mesh.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {
Mesh::Mesh() : model_(nullptr), meshType_(MESH_TYPE_STATIC), vertices_(nullptr), normals_(nullptr), texCoords_(nullptr),
               tangents_(nullptr), numVertices_(0), numNormals_(0), numTexCoords_(0), numTangents_(0), referencedVertices_(false),
               referencedNormals_(false), referencedTexCoords_(false), referencedTangents_(false), vbo_(nullptr), ibo_(nullptr), vao_(nullptr)
{
}

Mesh::Mesh(const string& filename, MeshType_t meshType) : model_(nullptr), meshType_(MESH_TYPE_STATIC), vertices_(nullptr), normals_(nullptr), texCoords_(nullptr),
               tangents_(nullptr), numVertices_(0), numNormals_(0), numTexCoords_(0), numTangents_(0), referencedVertices_(false),
               referencedNormals_(false), referencedTexCoords_(false), referencedTangents_(false), vbo_(nullptr), ibo_(nullptr), vao_(nullptr)
{
    ResourceManager::GetInstance()->LoadModelGeometryFromFile(filename, model_);
    Initialize(meshType);
}

Mesh::~Mesh() {
    if (!referencedVertices_) {
        delete[] vertices_;
    }

    if (!referencedNormals_) {
        delete[] normals_;
    }

    if (!referencedTexCoords_) {
        delete[] texCoords_;
    }

    if (!referencedTangents_) {
        delete[] tangents_;
    }

    glDeleteBuffers(model_->size(), vbo_);
    glDeleteBuffers(model_->size(), ibo_);
    glDeleteVertexArrays(model_->size(), vao_);

    delete[] vbo_;
    delete[] ibo_;
    delete[] vao_;
}

void Mesh::Load(const string& filename) {

}

void Mesh::SetVertices(const vector<Vector3>& vertices) {
    if (!referencedVertices_ && vertices_ != nullptr) {
        delete[] vertices_;
    }
    numVertices_ = vertices.size();

    if (numVertices_ > 0) {
        vertices_ = new Vector3[numVertices_];
        memcpy(vertices_, &vertices[0], numVertices_ * sizeof(Vector3));

        referencedVertices_ = false;
    }
}

void Mesh::SetNormals(const vector<Vector3>& normals) {
    if (!referencedNormals_ && normals_ != nullptr) {
        delete[] normals_;
    }
    numNormals_ = normals.size();

    if (numNormals_ > 0) {
        normals_ = new Vector3[numNormals_];
        memcpy(normals_, &normals[0], numNormals_ * sizeof(Vector3));

        referencedNormals_ = false;
    }
}

void Mesh::SetTextureCoords(const vector<Vector2>& texCoords) {
    if (!referencedTexCoords_ && texCoords_ != nullptr) {
        delete[] texCoords_;
    }
    numTexCoords_ = texCoords.size();

    if (texCoords_ > 0) {
        texCoords_ = new Vector2[numTexCoords_];
        memcpy(texCoords_, &texCoords[0], numTexCoords_ * sizeof(Vector2));

        referencedTexCoords_ = false;
    }
}

void Mesh::SetTangents(const vector<Vector3>& tangents) {
    if (!referencedTangents_ && tangents_ != nullptr) {
        delete[] tangents_;
    }
    numTangents_ = tangents.size();

    if (numTangents_ > 0) {
        tangents_ = new Vector3[numTangents_];
        memcpy(tangents_, &tangents[0], numTangents_ * sizeof(Vector3));

        referencedTangents_ = false;
    }
}

void Mesh::Initialize(MeshType_t meshType) {
    meshType_ = meshType;

	// TEMP - construct the buffers
    vbo_ = new unsigned int[model_->size()];
    ibo_ = new unsigned int[model_->size()];
    vao_ = new unsigned int[model_->size()];

    glGenBuffers(model_->size(), vbo_);
    glGenBuffers(model_->size(), ibo_);
    glGenVertexArrays(model_->size(), vao_);

    for (size_t i = 0; i < model_->size(); i++) {
	    // Interleave the data
	    vector<float> data;
        data.reserve((*model_)[i]->vertices.size() + (*model_)[i]->normals.size() + (*model_)[i]->uvs.size());

	    size_t idx = 0;
        size_t uv = 0;
	    size_t numVertices = (*model_)[i]->vertices.size() / 3;
	    for (size_t j = 0; j < numVertices; j++) {
		    data.push_back((*model_)[i]->vertices[idx]); data.push_back((*model_)[i]->vertices[idx + 1]); data.push_back((*model_)[i]->vertices[idx + 2]);
		    data.push_back((*model_)[i]->normals[idx]); data.push_back((*model_)[i]->normals[idx + 1]); data.push_back((*model_)[i]->normals[idx + 2]);
            data.push_back((*model_)[i]->uvs[uv]); data.push_back((*model_)[i]->uvs[uv + 1]);
		    idx += 3;
            uv += 2;
	    }

        // We first create the vertex array object and then bind the vertex and index buffer objects
	    glBindVertexArray(vao_[i]);

        // Vertex buffer object
        int type = (meshType_ == MESH_TYPE_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
	    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], type);

        glEnableVertexAttribArray(0);
	    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 32, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);

        // Index buffer object
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_[i]);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*model_)[i]->indices.size() * sizeof(unsigned int), &((*model_)[i]->indices)[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

bool Mesh::UpdateMeshData(vector<LoadedModel_t*>*& modelData) {
    if (meshType_ == MESH_TYPE_STATIC) {
        return false;
    }
    model_ = modelData;

    for (size_t i = 0; i < model_->size(); i++) {
	    // Interleave the data
	    vector<float> data;
        data.reserve((*model_)[i]->vertices.size() + (*model_)[i]->normals.size() + (*model_)[i]->uvs.size());

	    size_t idx = 0;
        size_t uv = 0;
	    size_t numVertices = (*model_)[i]->vertices.size() / 3;
	    for (size_t j = 0; j < numVertices; j++) {
		    data.push_back((*model_)[i]->vertices[idx]); data.push_back((*model_)[i]->vertices[idx + 1]); data.push_back((*model_)[i]->vertices[idx + 2]);
		    data.push_back((*model_)[i]->normals[idx]); data.push_back((*model_)[i]->normals[idx + 1]); data.push_back((*model_)[i]->normals[idx + 2]);
            data.push_back((*model_)[i]->uvs[uv]); data.push_back((*model_)[i]->uvs[uv + 1]);
		    idx += 3;
            uv += 2;
	    }

	    glBindVertexArray(vao_[i]);

        // Vertex buffer object
	    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
	    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 32, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}

void Mesh::GetRenderInfo(vector<LoadedModel_t*>*& modelData, unsigned int*& bufferObjects) const {
    modelData = model_;
    bufferObjects = vao_;
}

}