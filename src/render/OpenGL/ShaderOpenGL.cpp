#include "render/OpenGL/ShaderOpenGL.h"

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/Renderer.h"
#include "render/RenderSystem.h"

#include "system/Logger.h"

#include <fstream>
using namespace std;

namespace Sketch3D {

ShaderOpenGL::ShaderOpenGL(const string& vertexFilename,
						   const string& fragmentFilename,
                           const vector<string>& vertexInputs) : Shader(vertexFilename,
																	    fragmentFilename),
															     currentTextureUnit_(0)
{
	Logger::GetInstance()->Debug("Shader creation");

	program_ = glCreateProgram();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	if (!vertex) {
		Logger::GetInstance()->Error("Couldn't create vertex shader");
		return;
	}

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment) {
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

	glShaderSource(vertex, 1, &vertexFile, NULL);
	glCompileShader(vertex);
	glAttachShader(program_, vertex);
	LogShaderErrors(vertex, vertexFilename);

	// Create the fragment shader
	Logger::GetInstance()->Debug("Fragment shader creation");
	const char* fragmentFile = ReadShader(fragmentFilename);
    if (fragmentFile == nullptr) {
        Logger::GetInstance()->Error("Couldn't read fragment shader file " + fragmentFilename);
        return;
    }

	glShaderSource(fragment, 1, &fragmentFile, NULL);
	glCompileShader(fragment);
	glAttachShader(program_, fragment);
	LogShaderErrors(fragment, fragmentFilename);
	
	// TEMP
    for (size_t i = 0; i < vertexInputs.size(); i++) {
        glBindAttribLocation(program_, i, vertexInputs[i].c_str());
    }

	Logger::GetInstance()->Debug("Shader program linking");
	glLinkProgram(program_);
	LogProgramErrors();
}

void ShaderOpenGL::SetActive(bool val) {
    // TODO find a way to do that correctly
    /*
	if (val != active_) {
		active_ = val;
		if (active_) {
			glUseProgram(program_);
		} else {
			glUseProgram(0);
		}
	}*/
    if (val) {
        glUseProgram(program_);
    } else {
        glUseProgram(0);
    }
}

bool ShaderOpenGL::SetUniformInt(const string& uniform, int value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
        Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform1i(location, value);
    return true;
}

bool ShaderOpenGL::SetUniformFloat(const string& uniform, float value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform1f(location, value);
    return true;
}

bool ShaderOpenGL::SetUniformVector2(const string& uniform, float value1, float value2) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform2f(location, value1, value2);
    return true;
}

bool ShaderOpenGL::SetUniformVector3(const string& uniform, const Vector3& value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform3f(location, value.x, value.y, value.z);
    return true;
}

bool ShaderOpenGL::SetUniformVector4(const string& uniform, const Vector4& value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform4f(location, value.x, value.y, value.z, value.w);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	float data[9];
	value.GetData(data);
	glUniformMatrix3fv(location, 1, false, data);
    return true;
}

bool ShaderOpenGL::SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	float data[16];
	value.GetData(data);
	glUniformMatrix4fv(location, 1, false, data);
    return true;
}

bool ShaderOpenGL::SetUniformTexture(const string& uniform, int activeTexture) {
	GLint location = glGetUniformLocation(program_, uniform.c_str());
	if (location == -1) {
		Logger::GetInstance()->Warning("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(program_));
		return false;
	}

	glUniform1i(location, activeTexture);
    return true;
}

bool ShaderOpenGL::SelectSubroutine(const string& subroutine, ShaderType_t type) {
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
        Logger::GetInstance()->Warning("Couldn't find subroutine of name " + subroutine + " in shader #" + to_string(program_));
        return false;
    }

    glUniformSubroutinesuiv(shader, 1, &index);
    return true;
}

char* ShaderOpenGL::ReadShader(const string& filename) {
    char* content = NULL;
	FILE* fp;
    errno_t err = fopen_s(&fp, filename.c_str(), "r");

	if (err != 0) {
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

        if (fp != nullptr) {
		    fclose(fp);
        }
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