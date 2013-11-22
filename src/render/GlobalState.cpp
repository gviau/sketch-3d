#include "GlobalState.h"

namespace Sketch3D
{
AlphaState::AlphaState()
{
    blendEnabled = false;
    srcBlend = SRC_ALPHA;
    dstBlend = ONE_MINUS_SRC_ALPHA;

    testEnabled = false;
    test = ALWAYS;
    reference = 0.0f;
}

AlphaState::globalState_t AlphaState::GetType() const
{
    return ALPHA;
}

GlobalState* AlphaState::CreateCopy() const
{
    return new AlphaState((*this));
}

CullState::CullState()
{
    enabled = true;
    frontFace = CCW;
    cullFace = BACK;
}

CullState::globalState_t CullState::GetType() const
{
    return CULL;
}

GlobalState* CullState::CreateCopy() const
{
    return new CullState((*this));
}

FogState::FogState()
{
    enabled = false;
    start = 0.0f;
    end = 1.0f;
    power = 1.0f;
    density = LINEAR;
    apply = PER_VERTEX;
}

FogState::globalState_t FogState::GetType() const
{
    return FOG;
}

GlobalState* FogState::CreateCopy() const
{
    return new FogState((*this));
}

MaterialState::MaterialState() : ambient(Vector4(0.2f, 0.2f, 0.2f)),
                                 diffuse(Vector4(0.8f, 0.8f, 0.8f))
{
    shininess = 1.0f;
}

MaterialState::globalState_t MaterialState::GetType() const
{
    return MATERIAL;
}

GlobalState* MaterialState::CreateCopy() const
{
    return new MaterialState((*this));
}

WireframeState::WireframeState()
{
    enabled = false;
}

WireframeState::globalState_t WireframeState::GetType() const
{
    return WIREFRAME;
}

GlobalState* WireframeState::CreateCopy() const
{
    return new WireframeState((*this));
}

ZBufferState::ZBufferState()
{
    enabled = true;
    writable = true;
    compare = LEQUAL;
}

ZBufferState::globalState_t ZBufferState::GetType() const
{
    return ZBUFFER;
}

GlobalState* ZBufferState::CreateCopy() const
{
    return new ZBufferState((*this));
}
}
