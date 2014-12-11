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

ShaderOpenGL::ShaderOpenGL(const string& vertexFilename, const string& fragmentFilename,
                           const vector<string>& vertexInputs) : Shader(vertexFilename, fragmentFilename),
															     vertex_(0),
                                                                 fragment_(0)
{
	Logger::GetInstance()->Debug("OpenGL shader creation");

	program_ = glCreateProgram();

	vertex_ = glCreateShader(GL_VERTEX_SHADER);
	if (!vertex_) {
		Logger::GetInstance()->Error("Couldn't create vertex shader");
		return;
	}

	fragment_ = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_) {
		Logger::GetInstance()->Error("Couldn't create fragment shader");
		return;
	}

	// Create the vertex shader
	Logger::GetInstance()->Debug("Vertex shader creation");
	const char* vertexFile = ReadShader(vertexFilename);
    if (vertexFile == nullptr) {
        Logger::GetInstance()->Error("Couldn't read vertex shader file " + vertexFilename);
        return;
    }

	glShaderSource(vertex_, 1, &vertexFile, NULL);
	glCompileShader(vertex_);
	glAttachShader(program_, vertex_);
	LogShaderErrors(vertex_, vertexFilename);

	// Create the fragment shader
	Logger::GetInstance()->Debug("Fragment shader creation");
	const char* fragmentFile = ReadShader(fragmentFilename);
    if (fragmentFile == nullptr) {
        Logger::GetInstance()->Error("Couldn't read fragment shader file " + fragmentFilename);
        return;
    }

	glShaderSource(fragment_, 1, &fragmentFile, NULL);
	glCompileShader(fragment_);
	glAttachShader(program_, fragment_);
	LogShaderErrors(fragment_, fragmentFilename);
	
    // Sets the attrib location according the array of vertex inputs
    for (size_t i = 0; i < vertexInputs.size(); i++) {
        glBindAttribLocation(program_, i, vertexInputs[i].c_str());
    }

	Logger::GetInstance()->Debug("Shader program linking");
	glLinkProgram(program_);
	LogProgramErrors();
}

ShaderOpenGL::~ShaderOpenGL() {
    glDeleteProgram(program_);
    glDeleteShader(vertex_);
    glDeleteShader(fragment_);
}

bool ShaderOpenGL::SetUniformInt(const string& uniformName, int value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform1i(location, value);
    return true;
}

bool ShaderOpenGL::SetUniformFloat(const string& uniformName, float value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform1f(location, value);
    return true;
}

bool ShaderOpenGL::SetUniformVector2(const string& uniformName, float value1, float value2) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform2f(location, value1, value2);
    return true;
}

bool ShaderOpenGL::SetUniformVector3(const string& uniformName, const Vector3& value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform3f(location, value.x, value.y, value.z);
    return true;
}

bool ShaderOpenGL::SetUniformVector4(const string& uniformName, const Vector4& value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    glUniform4f(location, value.x, value.y, value.z, value.w);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix3x3(const string& uniformName, const Matrix3x3& value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    float mat[9];
    value.GetData(mat);
    glUniformMatrix3fv(location, 1, false, mat);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix4x4(const string& uniformName, const Matrix4x4& value) {
    GLint location = glGetUniformLocation(program_, uniformName.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
        return false;
	}

    float mat[16];
    value.GetData(mat);
    glUniformMatrix4fv(location, 1, false, mat);
    return true;
}

bool ShaderOpenGL::SetUniformTexture(const string& uniform, const Texture* texture) {
    GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
        Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(id_));
        return false;
	}

    // Bind the texture and send its texture unit to the shader
    size_t textureUnit = texture->Bind();
    glUniform1i(location, textureUnit);

    return true;
}

bool ShaderOpenGL::SelectSubroutine(const string& subroutine, ShaderType_t type) {
    Renderer::GetInstance()->BindShader(this);

    GLint shader;
    switch (type) {
        case SHADER_TYPE_VERTEX:
            shader = GL_VERTEX_SHADER;
            break;

        case SHADER_TYPE_FRAGMENT:
            shader = GL_FRAGMENT_SHADER;
            break;
    }

    GLuint index = glGetSubroutineIndex(program_, shader, subroutine.c_str());
    if (index == GL_INVALID_INDEX) {
        Logger::GetInstance()->Debug("Couldn't find subroutine of name " + subroutine + " in shader #" + to_string(program_));
        return false;
    }

    glUniformSubroutinesuiv(shader, 1, &index);
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
