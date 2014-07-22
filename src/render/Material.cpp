#include "render/Material.h"

#include "render/Mesh.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include "system/Logger.h"

namespace Sketch3D {

Material::Material(Shader* shader) : shader_(shader) {
}

void Material::SetShader(Shader* shader) {
	shader_ = shader;
}

void Material::SetTextures(vector<Texture2D*>*& textures) {
    textures_ = textures;
}

Shader* Material::GetShader() const {
	return shader_;
}

const vector<Texture2D*>* Material::GetTextures() const {
    return textures_;
}

}