#ifndef SKETCH_3D_SHADER_H
#define SKETCH_3D_SHADER_H

#include <string>
using namespace std;

namespace Sketch3D {

// Forward declarations
class Matrix3x3;
class Matrix4x4;
class Vector3;
class Vector4;
class Texture2D;

/**
 * @class Shader
 * This class make it possible to use easily a set of shader to render an
 * object as well as to pass properties to the shader. It acts as an interface
 * for the DirectX and OpenGL type of shaders
 */
class Shader {
	public:
		/**
		 * Constructor. Load the vertex and fragment shader
		 * @param vertexFilename The filename of the vertex shader
		 * @param fragmentFilename The filename of the fragment shader
		 */
		Shader(const string& vertexFilename, const string& fragmentFilename) : active_(false) {}

		/**
		 * Activate or deactivate the shader
		 * @param val If set to true, this shader will be set as the current
		 * one and will be deactivated otherwise.
		 */
		virtual void	SetActive(bool val) = 0;

		// UNIFORM SETTERS
		virtual void	SetUniformInt(const string& uniform, int value) = 0;
		virtual void	SetUniformFloat(const string& uniform, float value) = 0;
		virtual void	SetUniformVector2(const string& uniform, float value1, float value2) = 0;
		virtual void	SetUniformVector3(const string& uniform, const Vector3& value) = 0;
		virtual void	SetUniformVector4(const string& uniform, const Vector4& value) = 0;
		virtual void	SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) = 0;
		virtual void	SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) = 0;
		virtual void	SetUniformTexture(const string& uniform, const Texture2D& value) = 0;

		bool			IsActive() { return active_; }

	protected:
		bool			active_;	/**< Is the shader active? */
};

}

#endif