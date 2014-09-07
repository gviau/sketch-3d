#include "render/Material.h"

#include "render/Mesh.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include "system/Logger.h"

namespace Sketch3D {

Material::Material(Shader* shader) : shader_(shader), transluencyType_(TRANSLUENCY_TYPE_OPAQUE) {
}

void Material::SetShader(Shader* shader) {
	shader_ = shader;
}

void Material::SetTextures(vector<vector<Texture2D*>>*& textures) {
    textures_ = textures;
}

void Material::SetTransluencyType(TransluencyType_t type) {
    transluencyType_ = type;
}

Shader* Material::GetShader() const {
	return shader_;
}

const vector<vector<Texture2D*>>* Material::GetTextures() const {
    return textures_;
}

TransluencyType_t Material::GetTransluencyType() const {
    return transluencyType_;
}

}