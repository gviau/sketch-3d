//#ifndef SKETCH_3D_MATERIAL_H
//#define SKETCH_3D_MATERIAL_H
//
//#include "render/Shader.h"
//
//#include "system/Platform.h"
//
//#include <map>
//#include <string>
//#include <utility>
//#include <vector>
//using namespace std;
//
//namespace Sketch3D {
//
//// Forward declarations
//class Mesh;
//class Texture;
//
//enum TransluencyType_t {
//    TRANSLUENCY_TYPE_OPAQUE,
//    TRANSLUENCY_TYPE_ADDITIVE
//};
//
///**
// * @class Material
// * This class defines a material to use on a mesh. It gives has a shader that
// * will be run to render the mesh as well as a set of general material
// * properties that will be pass to the shader to control how the mesh will be
// * rendered.
// */
//class SKETCH_3D_API Material {
//	public:
//		/**
//		 * Constructor. Sets the shader for the material
//		 */
//					                    Material(Shader* shader);
//
//        /**
//         * Apply the material's uniform to its shader
//         * @return false if the material couldn't apply its uniform, true otherwise
//         */
//        bool                            ApplyMaterial() const;
//
//        void		                    SetShader(Shader* shader);
//        void                            SetTransluencyType(TransluencyType_t type);
//
//        // UNIFORM SETTERS
//        void	                        SetUniformInt(const string& uniform, int value);
//        void	                        SetUniformFloat(const string& uniform, float value);
//        void	                        SetUniformVector2(const string& uniform, float value1, float value2);
//        void	                        SetUniformVector3(const string& uniform, const Vector3& value);
//        void                            SetUniformVector3Array(const string& uniform, const vector<Vector3>& values);
//        void	                        SetUniformVector4(const string& uniform, const Vector4& value);
//        void	                        SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value);
//        void	                        SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value);
//        void                            SetUniformMatrix4x4Array(const string& uniform, const vector<Matrix4x4>& values);
//        void	                        SetUniformTexture(const string& uniform, const Texture* texture);
//
//		Shader*		                    GetShader() const;
//        TransluencyType_t               GetTransluencyType() const;
//
//	private:
//		Shader*		                    shader_;	/**< Shader used by the material */
//        TransluencyType_t               transluencyType_;   /**< The transluency type for this material */
//
//        // List of uniforms to set during rendering with this material
//        map<string, int>                intUniforms_;
//        map<string, float>              floatUniforms_;
//        map<string, Vector2>            vector2Uniforms_;
//        map<string, Vector3>            vector3Uniforms_;
//        map<string, vector<Vector3>>    vector3ArrayUniforms_;
//        map<string, Vector4>            vector4Uniforms_;
//        map<string, Matrix3x3>          matrix3x3Uniforms_;
//        map<string, Matrix4x4>          matrix4x4Uniforms_;
//        map<string, vector<Matrix4x4>>  matrix4x4ArrayUniforms_;
//        map<string, const Texture*>     textureUniforms_;
//};
//
//}
//
//#endif