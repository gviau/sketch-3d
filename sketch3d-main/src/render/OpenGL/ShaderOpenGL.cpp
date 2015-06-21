#include "render/OpenGL/ShaderOpenGL.h"

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/Renderer.h"
#include "render/RenderSystem.h"
#include "render/Texture.h"

#include "system/Logger.h"

#include <fstream>
using namespace std;

namespace Sketch3D {

ShaderOpenGL::ShaderOpenGL() : vertex_(0), fragment_(0) {
    Logger::GetInstance()->Debug("OpenGL Shader creation");
    program_ = glCreateProgram();
}

ShaderOpenGL::~ShaderOpenGL() {
    if (vertex_ != 0) {
        glDeleteShader(vertex_);
    }

    if (fragment_ != 0) {
        glDeleteShader(fragment_);
    }

    glDeleteProgram(program_);
}

bool ShaderOpenGL::SetSourceFile(const string& vertexFilename, const string& fragmentFilename) {
	vertex_ = glCreateShader(GL_VERTEX_SHADER);
	if (!vertex_) {
		Logger::GetInstance()->Error("Couldn't create vertex shader");
		return false;
	}

	fragment_ = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_) {
		Logger::GetInstance()->Error("Couldn't create fragment shader");
		return false;
	}

	// Create the vertex shader
	Logger::GetInstance()->Debug("Vertex shader creation");
	const char* vertexFile = ReadShader(vertexFilename + ".glsl");
    if (vertexFile == nullptr) {
        Logger::GetInstance()->Error("Couldn't read vertex shader file " + vertexFilename + ".glsl");
        return false;
    }

	glShaderSource(vertex_, 1, &vertexFile, NULL);
	glCompileShader(vertex_);
	glAttachShader(program_, vertex_);
	LogShaderErrors(vertex_, vertexFilename + ".glsl");

	// Create the fragment shader
	Logger::GetInstance()->Debug("Fragment shader creation");
	const char* fragmentFile = ReadShader(fragmentFilename + ".glsl");
    if (fragmentFile == nullptr) {
        Logger::GetInstance()->Error("Couldn't read fragment shader file " + fragmentFilename + ".glsl");
        return false;
    }

	glShaderSource(fragment_, 1, &fragmentFile, NULL);
	glCompileShader(fragment_);
	glAttachShader(program_, fragment_);
	LogShaderErrors(fragment_, fragmentFilename + ".glsl");

	Logger::GetInstance()->Debug("Shader program linking");
	glLinkProgram(program_);
	LogProgramErrors();

    GLint numActiveUniforms;
    char uniformName[256];
    glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    for (int i = 0; i < numActiveUniforms; i++) {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(program_, i, 256, &actualLength, &arraySize, &type, uniformName);
        string name = uniformName;
        
        // Arrays' name are messed up
        if (name.back() == ']') {
            name.pop_back();
            name.pop_back();
            name.pop_back();
        }

        nameToUniforms_[name] = glGetUniformLocation(program_, name.c_str());
    }

    return true;
}

bool ShaderOpenGL::SetSource(const string& vertexSource, const string& fragmentSource) {
	vertex_ = glCreateShader(GL_VERTEX_SHADER);
	if (!vertex_) {
		Logger::GetInstance()->Error("Couldn't create vertex shader");
		return false;
	}

	fragment_ = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_) {
		Logger::GetInstance()->Error("Couldn't create fragment shader");
		return false;
	}

	// Create the vertex shader
	Logger::GetInstance()->Debug("Vertex shader creation");
    const char* vertexSourceCode = vertexSource.c_str();
    glShaderSource(vertex_, 1, &vertexSourceCode, NULL);
	glCompileShader(vertex_);
	glAttachShader(program_, vertex_);
	LogShaderErrors(vertex_, "Vertex source");

	// Create the fragment shader
	Logger::GetInstance()->Debug("Fragment shader creation");
    const char* fragmentSourceCode = fragmentSource.c_str();
    glShaderSource(fragment_, 1, &fragmentSourceCode, NULL);
	glCompileShader(fragment_);
	glAttachShader(program_, fragment_);
	LogShaderErrors(fragment_, "Fragment source");

	Logger::GetInstance()->Debug("Shader program linking");
	glLinkProgram(program_);
	LogProgramErrors();

    GLint numActiveUniforms;
    char uniformName[256];
    glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    for (int i = 0; i < numActiveUniforms; i++) {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(program_, i, 256, &actualLength, &arraySize, &type, uniformName);
        string name = uniformName;
        
        // Arrays' name are messed up
        if (name.back() == ']') {
            name.pop_back();
            name.pop_back();
            name.pop_back();
        }

        nameToUniforms_[name] = glGetUniformLocation(program_, name.c_str());
    }

    return true;
}

bool ShaderOpenGL::SetUniformInt(const string& uniformName, int value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform1i(it->second, value);
    return true;
}

bool ShaderOpenGL::SetUniformFloat(const string& uniformName, float value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform1f(it->second, value);
    return true;
}

bool ShaderOpenGL::SetUniformVector2(const string& uniformName, float value1, float value2) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform2f(it->second, value1, value2);
    return true;
}

bool ShaderOpenGL::SetUniformVector3(const string& uniformName, const Vector3& value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform3f(it->second, value.x, value.y, value.z);
    return true;
}

bool ShaderOpenGL::SetUniformVector3Array(const string& uniformName, const Vector3* values, int arraySize) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform3fv(it->second, arraySize, (const GLfloat*) values);
    return true;
}

bool ShaderOpenGL::SetUniformVector4(const string& uniformName, const Vector4& value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform4f(it->second, value.x, value.y, value.z, value.w);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix3x3(const string& uniformName, const Matrix3x3& value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    float mat[9];
    value.GetData(mat);
    glUniformMatrix3fv(it->second, 1, false, mat);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix4x4(const string& uniformName, const Matrix4x4& value) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    float mat[16];
    value.GetData(mat);
    glUniformMatrix4fv(it->second, 1, false, mat);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix4x4Array(const string& uniformName, const Matrix4x4* values, int arraySize) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    // Convert the matrices to an array of floats
    vector<float> matrices;
    matrices.reserve(arraySize * 16);
    for (int i = 0; i < arraySize; i++) {
        float mat[16];
        values[i].GetData(mat);
        
        for (size_t j = 0; j < 16; j++) {
            matrices.push_back(mat[j]);
        }
    }

    glUniformMatrix4fv(it->second, arraySize, false, &matrices[0]);

    return true;
}

bool ShaderOpenGL::SetUniformTexture(const string& uniformName, const Texture* texture) {
    unordered_map<string, GLuint>::iterator it = nameToUniforms_.find(uniformName);
    if (it == nameToUniforms_.end()) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    // Bind the texture and send its texture unit to the shader
    size_t textureUnit = texture->Bind();
    glUniform1i(it->second, textureUnit);

    return true;
}

char* ShaderOpenGL::ReadShader(const string& filename) {
    char* content = nullptr;
	FILE* fp = fopen(filename.c_str(), "r");

	if (fp == nullptr) {
		Logger::GetInstance()->Error("Can't open file : " + filename);
	} else {
		fseek( fp, 0, SEEK_END );
		int size = ftell(fp);
		if (size > 0) {
			rewind(fp);
			content = new char[size + 1];
			size = fread(content, sizeof(char), size, fp);
			content[size] = '\0';
		}

	    fclose(fp);
	}

    return content;
}

void ShaderOpenGL::LogProgramErrors() const {
	int length = 0;
	glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

	if (length > 1) {
		char* infoLog = new char[length + 1];
		int charsWritten = 0;
		glGetProgramInfoLog(program_, length, &charsWritten, infoLog);
		Logger::GetInstance()->Error("Shader program error: " + string(infoLog));
		delete[] infoLog;
	}
}

void ShaderOpenGL::LogShaderErrors(GLuint obj, const string& filename) const {
	int length = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);

	if (length > 1) {
		char* infoLog = new char[length + 1];
		int charsWritten = 0;
		glGetShaderInfoLog(obj, length, &charsWritten, infoLog);
		Logger::GetInstance()->Error("Shader error for file : " + filename);
		Logger::GetInstance()->Error(string(infoLog));
		delete[] infoLog;
	}
}

}
