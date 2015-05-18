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

bool Shader::SetUniformInt(const string& uniform, int value) {
    bool setUniform = false;

    if (intUniforms_.find(uniform) != intUniforms_.end()) {
        if (intUniforms_[uniform] != value) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformIntImpl(uniform, value)) {
            return false;
        }

        intUniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformFloat(const string& uniform, float value) {
    bool setUniform = false;

    if (floatUniforms_.find(uniform) != floatUniforms_.end()) {
        if ( fabs(floatUniforms_[uniform] - value) >= EPSILON ) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformFloatImpl(uniform, value)) {
            return false;
        }

        floatUniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformVector2(const string& uniform, float value1, float value2) {
    bool setUniform = false;
    Vector2 value(value1, value2);

    if (vector2Uniforms_.find(uniform) != vector2Uniforms_.end()) {
        if (vector2Uniforms_[uniform] != value) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformVector2Impl(uniform, value1, value2)) {
            return false;
        }

        vector2Uniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformVector3(const string& uniform, const Vector3& value) {
    bool setUniform = false;

    if (vector3Uniforms_.find(uniform) != vector3Uniforms_.end()) {
        if (vector3Uniforms_[uniform] != value) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformVector3Impl(uniform, value)) {
            return false;
        }

        vector3Uniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformVector3Array(const string& uniform, const Vector3* values, int arraySize) {
    bool setUniform = false;
    Vector3Array_t value(values, arraySize);

    if (vector3ArrayUniforms_.find(uniform) != vector3ArrayUniforms_.end()) {
        const Vector3Array_t& oldArray = vector3ArrayUniforms_[uniform];

        if (arraySize == oldArray.second) {

            bool arrayEqual = true;
            for (int i = 0; i < arraySize; i++) {
                if (values[i] != oldArray.first[i]) {
                    arrayEqual = false;
                    break;
                }
            }

            setUniform = arrayEqual;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformVector3ArrayImpl(uniform, values, arraySize)) {
            return false;
        }

        vector3ArrayUniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformVector4(const string& uniform, const Vector4& value) {
    bool setUniform = false;

    if (vector4Uniforms_.find(uniform) != vector4Uniforms_.end()) {
        if (vector4Uniforms_[uniform] != value) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformVector4Impl(uniform, value)) {
            return false;
        }

        vector4Uniforms_[uniform] = value;
    }

    return true;
}

bool Shader::SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) {
    return SetUniformMatrix3x3Impl(uniform, value);
}

bool Shader::SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) {
    return SetUniformMatrix4x4Impl(uniform, value);
}

bool Shader::SetUniformMatrix4x4Array(const string& uniform, const Matrix4x4* values, int arraySize) {
    return SetUniformMatrix4x4ArrayImpl(uniform, values, arraySize);
}

bool Shader::SetUniformTexture(const string& uniform, const Texture* texture) {
    bool setUniform = false;

    if (textureUniforms_.find(uniform) != textureUniforms_.end()) {
        if (textureUniforms_[uniform]->GetId() != texture->GetId()) {
            setUniform = true;
        }
    } else {
        setUniform = true;
    }

    if (setUniform) {
        if (!SetUniformTextureImpl(uniform, texture)) {
            return false;
        }

        textureUniforms_[uniform] = texture;
    }

    return true;
}

void SetBuiltinUniformName(BuiltinUniform_t builtinUniform, const string& uniformName) {
    builtUniformNames[builtinUniform] = uniformName;
}

const string& GetBuiltinUniformName(BuiltinUniform_t builtinUniform) {
    return builtUniformNames[builtinUniform];
}

}