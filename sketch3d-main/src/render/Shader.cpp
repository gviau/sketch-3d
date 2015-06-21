#include "render/Shader.h"

#include "render/Texture.h"

#include "system/Logger.h"

namespace Sketch3D {

string builtUniformNames[] = {
    "model",
    "view",
    "transInvView",
    "projection",
    "modelView",
    "transInvModelView",
    "viewProjection",
    "modelViewProjection",
    "texture0",
    "texture1",
    "texture2",
    "texture3",
};

uint16_t Shader::nextAvailableId_ = 0;

Shader::Shader() : id_(MAX_SHADER_ID) {
    if (nextAvailableId_ == MAX_SHADER_ID) {
        Logger::GetInstance()->Error("Maximum number of shaders created (" + to_string(MAX_SHADER_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Shader::~Shader() {
}

void SetBuiltinUniformName(BuiltinUniform_t builtinUniform, const string& uniformName) {
    builtUniformNames[builtinUniform] = uniformName;
}

const string& GetBuiltinUniformName(BuiltinUniform_t builtinUniform) {
    return builtUniformNames[builtinUniform];
}

}