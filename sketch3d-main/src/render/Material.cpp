//#include "render/Material.h"
//
//#include "render/Mesh.h"
//#include "render/Renderer.h"
//#include "render/Shader.h"
//
//#include "system/Logger.h"
//
//namespace Sketch3D {
//
//Material::Material(Shader* shader) : shader_(shader), transluencyType_(TRANSLUENCY_TYPE_OPAQUE) {
//}
//
//bool Material::ApplyMaterial() const {
//    if (shader_ == nullptr) {
//        return false;
//    }
//
//    Renderer::GetInstance()->BindShader(shader_);
//
//    for (auto it = intUniforms_.begin(); it != intUniforms_.end(); ++it) {
//        shader_->SetUniformInt(it->first, it->second);
//    }
//
//    for (auto it = floatUniforms_.begin(); it != floatUniforms_.end(); ++it) {
//        shader_->SetUniformFloat(it->first, it->second);
//    }
//
//    for (auto it = vector2Uniforms_.begin(); it != vector2Uniforms_.end(); ++it) {
//        shader_->SetUniformVector2(it->first, it->second.x, it->second.y);
//    }
//
//    for (auto it = vector3Uniforms_.begin(); it != vector3Uniforms_.end(); ++it) {
//        shader_->SetUniformVector3(it->first, it->second);
//    }
//
//    for (auto it = vector3ArrayUniforms_.begin(); it != vector3ArrayUniforms_.end(); ++it) {
//        shader_->SetUniformVector3Array(it->first, &(it->second[0]), it->second.size());
//    }
//
//    for (auto it = vector4Uniforms_.begin(); it != vector4Uniforms_.end(); ++it) {
//        shader_->SetUniformVector4(it->first, it->second);
//    }
//
//    for (auto it = matrix3x3Uniforms_.begin(); it != matrix3x3Uniforms_.end(); ++it) {
//        shader_->SetUniformMatrix3x3(it->first, it->second);
//    }
//
//    for (auto it = matrix4x4Uniforms_.begin(); it != matrix4x4Uniforms_.end(); ++it) {
//        shader_->SetUniformMatrix4x4(it->first, it->second);
//    }
//
//    for (auto it = matrix4x4ArrayUniforms_.begin(); it != matrix4x4ArrayUniforms_.end(); ++it) {
//        shader_->SetUniformMatrix4x4Array(it->first, &(it->second[0]), it->second.size());
//    }
//
//    for (auto it = textureUniforms_.begin(); it != textureUniforms_.end(); ++it) {
//        shader_->SetUniformTexture(it->first, it->second);
//    }
//
//    return true;
//}
//
//void Material::SetShader(Shader* shader) {
//	shader_ = shader;
//}
//
//void Material::SetTransluencyType(TransluencyType_t type) {
//    transluencyType_ = type;
//}
//
//void Material::SetUniformInt(const string& uniform, int value) {
//    intUniforms_[uniform] = value;
//}
//
//void Material::SetUniformFloat(const string& uniform, float value) {
//    floatUniforms_[uniform] = value;
//}
//
//void Material::SetUniformVector2(const string& uniform, float value1, float value2) {
//    vector2Uniforms_[uniform] = Vector2(value1, value2);
//}
//
//void Material::SetUniformVector3(const string& uniform, const Vector3& value) {
//    vector3Uniforms_[uniform] = value;
//}
//
//void Material::SetUniformVector3Array(const string& uniform, const vector<Vector3>& values) {
//    vector3ArrayUniforms_[uniform] = values;
//}
//
//void Material::SetUniformVector4(const string& uniform, const Vector4& value) {
//    vector4Uniforms_[uniform] = value;
//}
//
//void Material::SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) {
//    matrix3x3Uniforms_[uniform] = value;
//}
//
//void Material::SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) {
//    matrix4x4Uniforms_[uniform] = value;
//}
//
//void Material::SetUniformMatrix4x4Array(const string& uniform, const vector<Matrix4x4>& values) {
//    matrix4x4ArrayUniforms_[uniform] = values;
//}
//
//void Material::SetUniformTexture(const string& uniform, const Texture* texture) {
//    textureUniforms_[uniform] = texture;
//}
//
//Shader* Material::GetShader() const {
//	return shader_;
//}
//
//TransluencyType_t Material::GetTransluencyType() const {
//    return transluencyType_;
//}
//
//}