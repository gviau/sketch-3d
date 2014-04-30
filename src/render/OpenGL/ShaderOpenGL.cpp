#include "render/OpenGL/ShaderOpenGL.h"

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "system/Logger.h"

#include <fstream>
using namespace std;

namespace Sketch3D {

ShaderOpenGL::ShaderOpenGL(const string& vertexFilename,
						   const string& fragmentFilename) : Shader(vertexFilename,
																	fragmentFilename)
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
	glShaderSource(vertex, 1, &vertexFile, NULL);
	glCompileShader(vertex);
	glAttachShader(program_, vertex);
	LogShaderErrors(vertex, vertexFilename);

	// Create the fragment shader
	Logger::GetInstance()->Debug("Fragment shader creation");
	const char* fragmentFile = ReadShader(fragmentFilename);
	glShaderSource(fragment, 1, &fragmentFile, NULL);
	glCompileShader(fragment);
	glAttachShader(program_, fragment);
	LogShaderErrors(fragment, fragmentFilename);
	
	// TEMP
	glBindAttribLocation(program_, 0, "in_vertex");
	glBindAttribLocation(program_, 1, "in_normal");

	Logger::GetInstance()->Debug("Shader program linking");
	glLinkProgram(program_);
	LogProgramErrors();
}

void ShaderOpenGL::SetActive(bool val) {
	if (val != active_) {
		active_ = val;
		if (active_) {
			glUseProgram(program_);
		} else {
			glUseProgram(0);
		}
	}
}

void ShaderOpenGL::SetUniformInt(const string& uniform, int value) {
	glUniform1i(glGetUniformLocation(program_, uniform.c_str()), value);
}

void ShaderOpenGL::SetUniformFloat(const string& uniform, float value) {
	glUniform1f(glGetUniformLocation(program_, uniform.c_str()), value);
}

void ShaderOpenGL::SetUniformVector2(const string& uniform, float value1, float value2) {
	glUniform2f(glGetUniformLocation(program_, uniform.c_str()), value1, value2);
}

void ShaderOpenGL::SetUniformVector3(const string& uniform, const Vector3& value) {
	glUniform3f(glGetUniformLocation(program_, uniform.c_str()), value.x, value.y, value.z);
}

void ShaderOpenGL::SetUniformVector4(const string& uniform, const Vector4& value) {
	glUniform4f(glGetUniformLocation(program_, uniform.c_str()), value.x, value.y, value.z, value.w);
}

void ShaderOpenGL::SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) {
	float data[9];
	value.GetData(data);
	glUniformMatrix3fv(glGetUniformLocation(program_, uniform.c_str()), 1, false, data);
}

void ShaderOpenGL::SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) {
	float data[16];
	value.GetData(data);
	glUniformMatrix4fv(glGetUniformLocation(program_, uniform.c_str()), 1, false, data);
}

char* ShaderOpenGL::ReadShader(const string& filename) {
    char* content = NULL;
	FILE* fp = fopen(filename.c_str(), "r");

	if (fp == NULL) {
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