#include "render/Material.h"

#include "render/Mesh.h"
#include "render/Shader.h"

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

Shader* Material::GetShader() const {
	return shader_;
}

}