#include "render/Mesh.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {
Mesh::Mesh() : model_(nullptr), meshType_(MESH_TYPE_STATIC), vbo_(nullptr), ibo_(nullptr), vao_(nullptr) {
}

Mesh::Mesh(const string& filename, MeshType_t meshType) : model_(nullptr), vbo_(nullptr), ibo_(nullptr), vao_(nullptr) {
    ResourceManager::GetInstance()->LoadModelGeometryFromFile(filename, model_);
    Initialize(model_, meshType);
}

void Mesh::Initialize(vector<LoadedModel_t*>*& modelData, MeshType_t meshType) {
    model_ = modelData;
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

Mesh::~Mesh() {
    glDeleteBuffers(model_->size(), vbo_);
    glDeleteBuffers(model_->size(), ibo_);
    glDeleteVertexArrays(model_->size(), vao_);

    delete[] vbo_;
    delete[] ibo_;
    delete[] vao_;
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