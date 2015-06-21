#ifndef SKETCH_3D_SHADER_H
#define SKETCH_3D_SHADER_H

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "system/Platform.h"

#include <map>
#include <stdint.h>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

#define MAX_SHADER_ID 4096  // 12 bits max

// Forward declarations
class Texture;

/**
 * @enum BuiltinUniform_t
 * Enumerates the different builtin uniform passed when rendering an object
 */
enum BuiltinUniform_t {
    MODEL,
    VIEW,
    TRANS_INV_VIEW,
    PROJECTION,
    MODEL_VIEW,
    TRANS_INV_MODEL_VIEW,
    VIEW_PROJECTION,
    MODEL_VIEW_PROJECTION,

    TEXTURE_0,
    TEXTURE_1,
    TEXTURE_2,
    TEXTURE_3,

    NUM_BUILTIN_UNIFORMS
};

/**
 * @enum ShaderType_t
 * Shows the possible shaders
 */
enum ShaderType_t {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
};

/**
 * @class Shader
 * This class make it possible to use easily a set of shader to render an
 * object as well as to pass properties to the shader. It acts as an interface
 * for the DirectX and OpenGL type of shaders
 */
class SKETCH_3D_API Shader {
    typedef pair<const Vector3*, int> Vector3Array_t;
    typedef pair<const Matrix4x4*, int> Matrix4x4Array_t;

	public:
		/**
		 * Constructor
		 */
		               Shader();

        /**
         * Destructor
         */
        virtual        ~Shader();

        /**
         * Set the source of the shader using text files
         * @param vertexFilename The name of the vertex shader file
         * @param fragmentFilename The name of the fragment shader file
         * @return true if the shaders were succesfully created, false otherwise
         */
        virtual bool    SetSourceFile(const string& vertexFilename, const string& fragmentFilename) = 0;

        /**
         * Set the source of the shader using in-memory source
         * @param vertexSource The source code of the vertex shader
         * @param fragmentSource The source code of the fragment shader
         * @return true if the shaders were succesfully created, false otherwise
         */
        virtual bool    SetSource(const string& vertexSource, const string& fragmentSource) = 0;

		// UNIFORM SETTERS
        virtual bool	SetUniformInt(const string& uniform, int value) = 0;
        virtual bool	SetUniformFloat(const string& uniform, float value) = 0;
        virtual bool	SetUniformVector2(const string& uniform, float value1, float value2) = 0;
        virtual bool	SetUniformVector3(const string& uniform, const Vector3& value) = 0;
        virtual bool    SetUniformVector3Array(const string& uniform, const Vector3* values, int arraySize) = 0;
        virtual bool	SetUniformVector4(const string& uniform, const Vector4& value) = 0;
        virtual bool	SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value) = 0;
        virtual bool	SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value) = 0;
        virtual bool    SetUniformMatrix4x4Array(const string& uniform, const Matrix4x4* values, int arraySize) = 0;
        virtual bool	SetUniformTexture(const string& uniform, const Texture* texture) = 0;

        uint16_t        GetId() const { return id_; }

	protected:
        uint16_t        id_;                /**< Id of the shader */
        static uint16_t nextAvailableId_;
};

extern "C" {
    SKETCH_3D_API void SetBuiltinUniformName(BuiltinUniform_t builtinUniform, const string& uniformName);
    SKETCH_3D_API const string& GetBuiltinUniformName(BuiltinUniform_t builtinUniform);
}

}

#endif