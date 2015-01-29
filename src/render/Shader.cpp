#include "render/Shader.h"

#include "system/Logger.h"

namespace Sketch3D {

uint16_t Shader::nextAvailableId_ = 0;

Shader::Shader(const string& vertexFilename, const string& fragmentFilename) : id_(MAX_SHADER_ID) {
    if (nextAvailableId_ == MAX_SHADER_ID) {
        Logger::GetInstance()->Error("Maximum number of shaders created (" + to_string(MAX_SHADER_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Shader::~Shader() {
}

}