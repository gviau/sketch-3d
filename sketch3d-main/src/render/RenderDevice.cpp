#include "render/RenderDevice.h"

#include "render/OpenGL/RenderDeviceOpenGL.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/Direct3D11/RenderDeviceDirect3D11.h"
#endif

#include "system/Logger.h"

namespace Sketch3D {
RenderDevice::~RenderDevice() {
}

shared_ptr<RenderDevice> SKETCH_3D_API CreateRenderDevice(RenderSystem_t renderSystem) {
    // If we are on a Unix system, we can only create an opengl render context
    RenderDevice* renderDevice = nullptr;

#if PLATFORM == PLATFORM_WIN32
    switch (renderSystem) {
        case RenderSystem_t::DIRECT3D11:    renderDevice = new RenderDeviceDirect3D11; break;
        case RenderSystem_t::OPENGL:        renderDevice = new RenderDeviceOpenGL; break;
    }
#elif PLATFORM == PLATFORM_LINUX
    renderDevice = new RenderDeviceOpenGL;
#endif

    if (renderDevice == nullptr) {
        Logger::GetInstance()->Error("Couldn't create the RenderDevice : Unknown render system");
    }

    return shared_ptr<RenderDevice>(renderDevice);
}
}