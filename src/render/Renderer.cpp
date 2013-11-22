#include "Renderer.h"

#include "RenderSystem.h"

namespace Sketch3D
{
Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(NULL)
{
}

Renderer::~Renderer()
{
    delete renderSystem_;
    renderSystem_ = NULL;
}

Renderer::Renderer(Renderer& src)
{
}

Renderer& Renderer::operator=(Renderer& rhs)
{
    return *this;
}

Renderer* Renderer::GetInstance()
{
    return &instance_;
}

void Renderer::SetRenderSystem(RenderSystem* renderSystem)
{
    delete renderSystem_;
    renderSystem = renderSystem;
}

SceneGraph& Renderer::GetSceneGraph()
{
    return sceneGraph_;
}
}
