#include "render/Mesh.h"

#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {
Mesh::Mesh() {
}

Mesh::Mesh(const string& filename) {
}

void Mesh::Render() const {
	// TODO - change that

}

Mesh Mesh::CreateCube(float width, float height, float depth) {
	Mesh cube;

	cube.vertices_.push_back(Vector3(-0.5f * width, -0.5f * height, -0.5f * depth));
	cube.vertices_.push_back(Vector3(-0.5f * width,  0.5f * height, -0.5f * depth));
	cube.vertices_.push_back(Vector3( 0.5f * width,  0.5f * height, -0.5f * depth));
	cube.vertices_.push_back(Vector3( 0.5f * width, -0.5f * height, -0.5f * depth));
	cube.vertices_.push_back(Vector3(-0.5f * width, -0.5f * height,  0.5f * depth));
	cube.vertices_.push_back(Vector3(-0.5f * width,  0.5f * height,  0.5f * depth));
	cube.vertices_.push_back(Vector3( 0.5f * width,  0.5f * height,  0.5f * depth));
	cube.vertices_.push_back(Vector3( 0.5f * width, -0.5f * height,  0.5f * depth));

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

	return cube;
}

}