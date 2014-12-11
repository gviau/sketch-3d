#ifndef SKETCH_3D_SHADER_DIRECT3D9_H
#define SKETCH_3D_SHADER_DIRECT3D9_H

#include "render/Shader.h"

// Forward class declaration
struct IDirect3DDevice9;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
struct ID3DXBuffer;
struct ID3DXConstantTable;

namespace Sketch3D {

/**
 * @class ShaderDirect3D9
 * Direct3D9 implementation of a shader object
 */
class ShaderDirect3D9 : public Shader {
    friend class RenderSystemDirect3D9;

    public:
                                ShaderDirect3D9(const string& vertexFilename, const string& fragmentFilename, IDirect3DDevice9* device);
        virtual                ~ShaderDirect3D9();

		virtual bool SetUniformInt(const string& uniform, int value);
		virtual bool SetUniformFloat(const string& uniform, float value);
		virtual bool SetUniformVector2(const string& uniform, float value1, float value2);
		virtual bool SetUniformVector3(const string& uniform, const Vector3& value);
		virtual bool SetUniformVector4(const string& uniform, const Vector4& value);
		virtual bool SetUniformMatrix3x3(const string& uniform, const Matrix3x3& value);
		virtual bool SetUniformMatrix4x4(const string& uniform, const Matrix4x4& value);
		virtual bool SetUniformTexture(const string& uniform, const Texture* texture);

    private:
        IDirect3DDevice9*       device_;
        IDirect3DVertexShader9* vertexShader_;
        IDirect3DPixelShader9*  fragmentShader_;
        ID3DXConstantTable*     vertexConstants_;
        ID3DXConstantTable*     fragmentConstants_;
};

}

#endif