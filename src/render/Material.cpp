#include "render/Material.h"

#include "render/Mesh.h"
#include "render/Shader.h"

#include "system/Logger.h"

namespace Sketch3D {

Material::Material(Shader* shader) : shader_(shader), transluencyType_(TRANSLUENCY_TYPE_OPAQUE) {
}

void Material::SetShader(Shader* shader) {
	shader_ = shader;
}

bool Material::AddTexture(const string& textureName, Texture* texture) {
    bool firstTexture = (textures_.find(textureName) == textures_.end());
    textures_[textureName] = texture;
    return firstTexture;
}

void Material::SetTransluencyType(TransluencyType_t type) {
    transluencyType_ = type;
}

Shader* Material::GetShader() const {
	return shader_;
}

const map<string, Texture*>& Material::GetTextures() const {
    return textures_;
}

TransluencyType_t Material::GetTransluencyType() const {
    return transluencyType_;
}

}