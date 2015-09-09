#ifndef SKETCH_3D_MATERIAL_H
#define SKETCH_3D_MATERIAL_H

#include "system/Platform.h"

#include "math/Vector3.h"
#include "math/Vector4.h"

#include <memory>
using namespace std;

namespace Sketch3D {

// Forward class declarations
class RenderDevice;
class SamplerState;
class Texture2D;

class SKETCH_3D_API Material {
    public:
                                        Material();

        bool                            Apply(const shared_ptr<RenderDevice>& renderDevice) const;

        void                            SetAmbientColor(const Vector3& color);
        void                            SetDiffuseColor(const Vector3& color);
        void                            SetSpecularColor(const Vector3& color);
        void                            SetSpecularPower(float power);
        void                            SetAmbientTexture(const shared_ptr<Texture2D>& texture);
        void                            SetAmbientSamplerState(const shared_ptr<SamplerState>& samplerState);
        void                            SetDiffuseTexture(const shared_ptr<Texture2D>& texture);
        void                            SetDiffuseSamplerState(const shared_ptr<SamplerState>& samplerState);
        void                            SetSpecularTexture(const shared_ptr<Texture2D>& texture);
        void                            SetSpecularSamplerState(const shared_ptr<SamplerState>& samplerState);
        void                            SetNormalMapTexture(const shared_ptr<Texture2D>& texture);
        void                            SetNormalMapSamplerState(const shared_ptr<SamplerState>& samplerState);

        const Vector3&                  GetAmbientColor() const;
        const Vector3&                  GetDiffuseColor() const;
        const Vector3&                  GetSpecularColor() const;
        float                           GetSpecularPower() const;
        const shared_ptr<Texture2D>&    GetAmbientTexture() const;
        const shared_ptr<SamplerState>& GetAmbientSamplerState() const;
        const shared_ptr<Texture2D>&    GetDiffuseTexture() const;
        const shared_ptr<SamplerState>& GetDiffuseSamplerState() const;
        const shared_ptr<Texture2D>&    GetSpecularTexture() const;
        const shared_ptr<SamplerState>& GetSpecularSamplerState() const;
        const shared_ptr<Texture2D>&    GetNormalMapTexture() const;
        const shared_ptr<SamplerState>& GetNormalMapSamplerState() const;

    private:
        Vector3                         m_AmbientColor;
        Vector3                         m_DiffuseColor;
        Vector3                         m_SpecularColor;
        float                           m_SpecularPower;

        shared_ptr<Texture2D>           m_AmbientTexture;
        shared_ptr<SamplerState>        m_AmbientSamplerState;
        shared_ptr<Texture2D>           m_DiffuseTexture;
        shared_ptr<SamplerState>        m_DiffuseSamplerState;
        shared_ptr<Texture2D>           m_SpecularTexture;
        shared_ptr<SamplerState>        m_SpecularSamplerState;
        shared_ptr<Texture2D>           m_NormalMapTexture;
        shared_ptr<SamplerState>        m_NormalMapSamplerState;
};

}

#endif