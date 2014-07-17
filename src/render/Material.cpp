#include "render/Material.h"

#include "render/Mesh.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include "system/Logger.h"

namespace Sketch3D {

Material::Material(Shader* shader) : shader_(shader) {
}

void Material::Apply(Mesh& mesh) const {
	if (!shader_) {
		Logger::GetInstance()->Warning("Shader pointer is null");
		return;
	}

	shader_->SetActive(true);
}

void Material::SetShader(Shader* shader) {
	shader_ = shader;
}

void Material::SetTexture(int num, Texture2D* texture) {
	switch (num) {
		case 1:
			texture1_ = texture;
			break;
		case 2:
			texture2_ = texture;
			break;
		case 3:
			texture3_ = texture;
			break;
		case 4:
			texture4_ = texture;
			break;
		default:
			break;
	}
}

Shader* Material::GetShader() const {
	return shader_;
}

Texture2D* Material::GetTexture(int num) const {
	Texture2D* texture = NULL;
	switch (num) {
		case 1:
			texture = texture1_;
			break;
		case 2:
			texture = texture2_;
			break;
		case 3:
			texture = texture3_;
			break;
		case 4:
			texture = texture4_;
			break;
		default:
			break;
	}

	return texture;
}

}