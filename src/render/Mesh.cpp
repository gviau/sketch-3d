#include "render/Mesh.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {
Mesh::Mesh() : vbo_(0), ibo_(0) {
}

Mesh::Mesh(const string& filename) : vbo_(0), ibo_(0) {
}

void Mesh::Render() const {
	// TODO - change that
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 24, 0);
	glColorPointer(3, GL_FLOAT, 24, (void*)12);

	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh Mesh::CreateCube(float width, float height, float depth) {
	Mesh cube;

	cube.vertices_.push_back(-0.5f * width); cube.vertices_.push_back(-0.5f * height); cube.vertices_.push_back( 0.5f * depth);
	cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f); cube.vertices_.push_back(0.0f);

	cube.vertices_.push_back(-0.5f * width); cube.vertices_.push_back( 0.5f * height); cube.vertices_.push_back( 0.5f * depth);
	cube.vertices_.push_back(0.0f); cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f);

	cube.vertices_.push_back( 0.5f * width); cube.vertices_.push_back( 0.5f * height); cube.vertices_.push_back( 0.5f * depth);
	cube.vertices_.push_back(0.0f); cube.vertices_.push_back(0.0f); cube.vertices_.push_back(1.0f);

	cube.vertices_.push_back( 0.5f * width); cube.vertices_.push_back(-0.5f * height); cube.vertices_.push_back( 0.5f * depth);
	cube.vertices_.push_back(1.0f); cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f);

	cube.vertices_.push_back(-0.5f * width); cube.vertices_.push_back(-0.5f * height); cube.vertices_.push_back(-0.5f * depth);
	cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f); cube.vertices_.push_back(1.0f);

	cube.vertices_.push_back(-0.5f * width); cube.vertices_.push_back( 0.5f * height); cube.vertices_.push_back(-0.5f * depth);
	cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f); cube.vertices_.push_back(0.0f);

	cube.vertices_.push_back( 0.5f * width); cube.vertices_.push_back( 0.5f * height); cube.vertices_.push_back(-0.5f * depth);
	cube.vertices_.push_back(0.0f); cube.vertices_.push_back(1.0f); cube.vertices_.push_back(0.0f);

	cube.vertices_.push_back( 0.5f * width); cube.vertices_.push_back(-0.5f * height); cube.vertices_.push_back(-0.5f * depth);
	cube.vertices_.push_back(0.0f); cube.vertices_.push_back(0.0f); cube.vertices_.push_back(1.0f);
	
	// Front face
	cube.indices_.push_back(0); cube.indices_.push_back(1); cube.indices_.push_back(2);
	cube.indices_.push_back(0); cube.indices_.push_back(2); cube.indices_.push_back(3);
	
	// Right face
	cube.indices_.push_back(3); cube.indices_.push_back(2); cube.indices_.push_back(6);
	cube.indices_.push_back(3); cube.indices_.push_back(6); cube.indices_.push_back(7);

	// Back face
	cube.indices_.push_back(7); cube.indices_.push_back(6); cube.indices_.push_back(5);
	cube.indices_.push_back(7); cube.indices_.push_back(5); cube.indices_.push_back(4);

	// Left face
	cube.indices_.push_back(4); cube.indices_.push_back(5); cube.indices_.push_back(1);
	cube.indices_.push_back(4); cube.indices_.push_back(1); cube.indices_.push_back(0);

	// Top face
	cube.indices_.push_back(1); cube.indices_.push_back(5); cube.indices_.push_back(6);
	cube.indices_.push_back(1); cube.indices_.push_back(6); cube.indices_.push_back(3);

	// Bottom face
	cube.indices_.push_back(4); cube.indices_.push_back(0); cube.indices_.push_back(3);
	cube.indices_.push_back(4); cube.indices_.push_back(3); cube.indices_.push_back(7);

	// TEMP
	glGenVertexArrays(1, &cube.vao_);
	glBindVertexArray(cube.vao_);

	glGenBuffers(1, &cube.vbo_);
	glGenBuffers(1, &cube.ibo_);

	glBindBuffer(GL_ARRAY_BUFFER, cube.vbo_);
	glBufferData(GL_ARRAY_BUFFER, cube.vertices_.size() * sizeof(GL_FLOAT), &(cube.vertices_)[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 24, 0);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 24, (void*)12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ibo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indices_.size() * sizeof(GL_FLOAT), &(cube.indices_)[0], GL_STATIC_DRAW);

	return cube;
}

}