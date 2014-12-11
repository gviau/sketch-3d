#ifndef SKETCH_3D_SHADER_OPENGL_H
#define SKETCH_3D_SHADER_OPENGL_H

#include "render/Shader.h"

#include "gl/glew.h"

#include <map>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @class ShaderOpenGL
 * Implement an OpenGL shader object
 */
class ShaderOpenGL : public Shader {
    friend class RenderSystemOpenGL;

	public:
		ShaderOpenGL(const string& vertexFilename, const string& fragmentFilename, const vector<string>& vertexInputs);
        virtual ~ShaderOpenGL();

		virtual bool SetUniformInt(const string& uniform, int value);
		virtual bool SetUniformFloat(const string& uniform, float value);
		virtual bool SetUniformVector2(const string& uniform, float value1, float value2);
		virtual bool SetUniformVector3(const string& uniform, const Vector3& value);
		virtual bool SetUniformVector4(const string& uniform, const Vector4& value);
		virtual bool SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value);
		virtual bool SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value);
		virtual bool SetUniformTexture(const string& uniform, const Texture* texture);
        virtual bool SelectSubroutine(const string& subroutine, ShaderType_t type);

	private:
        GLuint              vertex_;    /**< Represents the vertex shader */
        GLuint              fragment_;  /**< Represents the fragment shader */
		GLuint				program_;	/**< Represents the shader program */
		map<GLint, GLuint>	textures_;	/**< Allows the shader to map textures to its different texture units */

		/**
		 * Read the shader file and output a string to pass to the GLSL
		 * compiler
		 * @param filename The name of the file to read
		 * @return A single string containing the content of the file
		 */
		char*	ReadShader(const string& filename);

		/**
		 * Log the errors reported by the program
		 */
		void	LogProgramErrors() const;

		/**
		 * Log the errors reported by the shader
		 * @param obj The object for which the errors will be reported
		 * @param filename The name of the file that represents the shader
		 */
		void	LogShaderErrors(GLuint obj, const string& filename) const;
};

}

#endif