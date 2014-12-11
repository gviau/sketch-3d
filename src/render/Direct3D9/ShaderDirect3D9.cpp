#include "render/Direct3D9/ShaderDirect3D9.h"

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "system/Logger.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Sketch3D {

ShaderDirect3D9::ShaderDirect3D9(const string& vertexFilename, const string& fragmentFilename,
                                 IDirect3DDevice9* device) : Shader(vertexFilename, fragmentFilename),
                                                             device_(device)
{
    Logger::GetInstance()->Debug("Direct3D9 shader creation");

    DWORD flags = 0;

#ifdef _DEBUG
    flags = D3DXSHADER_DEBUG;
#else
    flags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

    ID3DXBuffer* shader = nullptr;
    ID3DXBuffer* errorBuffer = nullptr;

    Logger::GetInstance()->Debug("Vertex shader creation");

    HRESULT hr = D3DXCompileShaderFromFile(vertexFilename.c_str(), nullptr, nullptr, "main", "vs_3_0", flags,
                                           &shader, &errorBuffer, &vertexConstants_);
    if (errorBuffer != nullptr) {
        Logger::GetInstance()->Error("Shader error for file : " + vertexFilename);
        Logger::GetInstance()->Error((char*)errorBuffer->GetBufferPointer());
        errorBuffer->Release();
        errorBuffer = nullptr;
    }

    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't compile vertex shader : " + vertexFilename);

        if (shader != nullptr) {
            shader->Release();
        }

        return;
    }

    device_->CreateVertexShader((DWORD*)shader->GetBufferPointer(), &vertexShader_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create vertex shader : " + vertexFilename);

        if (shader != nullptr) {
            shader->Release();
        }

        return;
    }

    shader->Release();
    shader = nullptr;

    Logger::GetInstance()->Debug("Fragment shader creation");

    hr = D3DXCompileShaderFromFile(fragmentFilename.c_str(), nullptr, nullptr, "main", "ps_3_0", flags,
                                   &shader, &errorBuffer, &fragmentConstants_);
    if (errorBuffer != nullptr) {
        Logger::GetInstance()->Error("Shader error for file : " + vertexFilename);
        Logger::GetInstance()->Error((char*)errorBuffer->GetBufferPointer());
        errorBuffer->Release();
        errorBuffer = nullptr;
    }

    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't compile fragment shader : " + fragmentFilename);

        if (shader != nullptr) {
            shader->Release();
        }

        return;
    }

    device_->CreatePixelShader((DWORD*)shader->GetBufferPointer(), &fragmentShader_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create fragment shader : " + fragmentFilename);

        if (shader != nullptr) {
            shader->Release();
        }

        return;
    }

    shader->Release();
    shader = nullptr;
}

ShaderDirect3D9::~ShaderDirect3D9() {
    if (vertexShader_ != nullptr) {
        vertexShader_->Release();
    }

    if (fragmentShader_ != nullptr) {
        fragmentShader_->Release();
    }

    if (vertexConstants_ != nullptr) {
        vertexConstants_->Release();
    }

    if (fragmentConstants_ != nullptr) {
        fragmentConstants_->Release();
    }
}

bool ShaderDirect3D9::SetUniformInt(const string& uniformName, int value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        fragmentConstants_->SetInt(device_, uniformHandle, value);
    } else {
        vertexConstants_->SetInt(device_, uniformHandle, value);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformFloat(const string& uniformName, float value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        fragmentConstants_->SetFloat(device_, uniformHandle, value);
    } else {
        vertexConstants_->SetFloat(device_, uniformHandle, value);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformVector2(const string& uniformName, float value1, float value2) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        float values[] = { value1, value2 };
        fragmentConstants_->SetFloatArray(device_, uniformHandle, values, 2);
    } else {
        float values[] = { value1, value2 };
        vertexConstants_->SetFloatArray(device_, uniformHandle, values, 2);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformVector3(const string& uniformName, const Vector3& value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        float values[] = { value.x, value.y, value.z };
        fragmentConstants_->SetFloatArray(device_, uniformHandle, values, 3);
    } else {
        float values[] = { value.x, value.y, value.z };
        vertexConstants_->SetFloatArray(device_, uniformHandle, values, 3);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformVector4(const string& uniformName, const Vector4& value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        float values[] = { value.x, value.y, value.z, value.w };
        fragmentConstants_->SetFloatArray(device_, uniformHandle, values, 4);
    } else {
        float values[] = { value.x, value.y, value.z, value.w };
        vertexConstants_->SetFloatArray(device_, uniformHandle, values, 4);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformMatrix3x3(const string& uniformName, const Matrix3x3& value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        float mat[9];
        value.GetData(mat);
        fragmentConstants_->SetFloatArray(device_, uniformHandle, mat, 9);
    } else {
        float mat[9];
        value.GetData(mat);
        vertexConstants_->SetFloatArray(device_, uniformHandle, mat, 9);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformMatrix4x4(const string& uniformName, const Matrix4x4& value) {
    D3DXHANDLE uniformHandle = vertexConstants_->GetConstantByName(0, uniformName.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniformName.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniformName + " in shader #" + to_string(id_));
            return false;
	    }

        float mat[16];
        value.GetData(mat);
        fragmentConstants_->SetFloatArray(device_, uniformHandle, mat, 16);
    } else {
        float mat[16];
        value.GetData(mat);
        vertexConstants_->SetFloatArray(device_, uniformHandle, mat, 16);
    }

    return true;
}

bool ShaderDirect3D9::SetUniformTexture(const string& uniform, const Texture* texture) {
    bool fromVertexShader = true;
    D3DXHANDLE uniformHandle = 0;

    uniformHandle = vertexConstants_->GetConstantByName(0, uniform.c_str());
    if (uniformHandle == 0) {
        uniformHandle = fragmentConstants_->GetConstantByName(0, uniform.c_str());

	    if (uniformHandle == 0) {
            Logger::GetInstance()->Debug("Couldn't find uniform location of name " + uniform + " in shader #" + to_string(id_));
            return false;
	    }

        fromVertexShader = false;
    }

    UINT samplerIndex = 10000;

    if (fromVertexShader) {
        samplerIndex = vertexConstants_->GetSamplerIndex(uniformHandle);
    } else {
        samplerIndex = fragmentConstants_->GetSamplerIndex(uniformHandle);
    }

    if (samplerIndex != 10000) {
        // device_->SetTexture(samplerIndex, 
    }

    return true;
}

}