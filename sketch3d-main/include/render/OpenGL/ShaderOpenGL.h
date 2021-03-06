#ifndef SKETCH_3D_SHADER_OPENGL_H
#define SKETCH_3D_SHADER_OPENGL_H

#include "render/Shader.h"

#include "gl/glew.h"

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @class ShaderOpenGL
 * ement an OpenGL shader object
 */
class ShaderOpenGL : public Shader {
    friend class RenderSystemOpenGL;

	public:
		ShaderOpenGL();
        virtual ~ShaderOpenGL();

        virtual bool SetSourceFile(const string& vertexFilename, const string& fragmentFilename);
        virtual bool SetSource(const string& vertexSource, const string& fragmentSource);
        virtual bool SetUniformInt(const string& uniform, int value);
        virtual bool SetUniformFloat(const string& uniform, float value);
        virtual bool SetUniformVector2(const string& uniform, float value1, float value2);
        virtual bool SetUniformVector3(const string& uniform, const Vector3& value);
        virtual bool SetUniformVector3Array(const string& uniform, const Vector3* values, int arraySize);
        virtual bool SetUniformVector4(const string& uniform, const Vector4& value);
        virtual bool SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value);
        virtual bool SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value);
        virtual bool SetUniformMatrix4x4Array(const string& uniform, const Matrix4x4* values, int arraySize);
        virtual bool SetUniformTexture(const string& uniform, const Texture* texture);

	private:
        GLuint                          vertex_;    /**< Represents the vertex shader */
        GLuint                          fragment_;  /**< Represents the fragment shader */
		GLuint				            program_;	/**< Represents the shader program */
		map<GLint, GLuint>	            textures_;	/**< Allows the shader to map textures to its different texture units */
        unordered_map<string, GLuint>   nameToUniforms_;    /**< Map of uniform names to location */

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
