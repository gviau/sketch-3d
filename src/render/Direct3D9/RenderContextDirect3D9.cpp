#include "render/Direct3D9/RenderContextDirect3D9.h"

#include "system/Logger.h"
#include "system/Window.h"

#include <d3d9.h>

namespace Sketch3D {
RenderContextDirect3D9::RenderContextDirect3D9() : direct3D9Context_(nullptr), direct3D9Device_(nullptr) {
}

RenderContextDirect3D9::~RenderContextDirect3D9() {
    if (direct3D9Context_ != nullptr) {
        direct3D9Context_->Release();
    }
}

bool RenderContextDirect3D9::Initialize(Window& window) {
    Logger::GetInstance()->Debug("Initializing Direct3D9 context");

    direct3D9Context_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3D9Context_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create Direct3D9 context");
        return false;
    }

    HWND hWnd = reinterpret_cast<HWND>(window.GetHandle());
    D3DFORMAT format = D3DFMT_X8R8G8B8;

    D3DPRESENT_PARAMETERS presentParameters;
    ZeroMemory(&presentParameters, sizeof(presentParameters));
    presentParameters.BackBufferCount = 1;
    presentParameters.BackBufferFormat = format;
    presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParameters.MultiSampleQuality = 0;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.hDeviceWindow = hWnd;
    presentParameters.Flags = 0;
    presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;

    if (window.IsWindowed()) {
        presentParameters.Windowed = TRUE;
    } else {
        presentParameters.Windowed = FALSE;
        presentParameters.BackBufferWidth = window.GetWidth();
        presentParameters.BackBufferHeight = window.GetHeight();
    }

    HRESULT hr = direct3D9Context_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                                 &presentParameters, &direct3D9Device_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create Direct3D9 device");
        return false;
    }

    return true;
}

void RenderContextDirect3D9::SwapBuffers() {
    direct3D9Device_->Present(nullptr, nullptr, nullptr, nullptr);
}

IDirect3DDevice9* RenderContextDirect3D9::GetDevice() const {
    return direct3D9Device_;
}

}