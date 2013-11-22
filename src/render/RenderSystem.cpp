#include "RenderSystem.h"

namespace Sketch3D
{
RenderSystem::RenderSystem(const RenderWindow& window) : window_(window)
{
    Init();
}

RenderSystem::~RenderSystem()
{
    Shutdown();
}

void RenderSystem::Init()
{
    // Should be implemented in the sub classes
}

void RenderSystem::Shutdown()
{
    // Should be implemented in the sub classes
}

void RenderSystem::SetTransformMatrix(const Matrix4x4& transform)
{
    currentTransformMatrix_ = transform;
}

void RenderSystem::SetRenderState(const RenderState* renderState)
{
    // This is ugly
    currentRenderState_ = const_cast<RenderState*>(renderState);
}
}
