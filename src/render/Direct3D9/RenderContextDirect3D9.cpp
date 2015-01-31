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

bool RenderContextDirect3D9::Initialize(Window& window, const RenderParameters_t& renderParameters) {
    Logger::GetInstance()->Debug("Initializing Direct3D9 context");

    direct3D9Context_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3D9Context_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create Direct3D9 context");
        return false;
    }

    // Support only one monitor
    QueryAdapterSupportedDisplayFormats();

    if (!AreRenderParametersSupported(renderParameters)) {
        Logger::GetInstance()->Error("Render parameters are not supported by this display adapter");
        return false;
    }

    HWND hWnd = reinterpret_cast<HWND>(window.GetHandle());
    D3DFORMAT format;
    switch (renderParameters.displayFormat) {
        case DISPLAY_FORMAT_A1R5G5B5:       format = D3DFMT_A1R5G5B5; break;
        case DISPLAY_FORMAT_A2R10G10B10:    format = D3DFMT_A2R10G10B10; break;
        case DISPLAY_FORMAT_A8R8G8B8:       format = D3DFMT_A8R8G8B8; break;
        case DISPLAY_FORMAT_R5G6B5:         format = D3DFMT_R5G6B5; break;
        case DISPLAY_FORMAT_X1R5G5B5:       format = D3DFMT_X1R5G5B5; break;
        case DISPLAY_FORMAT_X8R8G8B8:       format = D3DFMT_X8R8G8B8; break;
    }

    D3DFORMAT depthStencilFormat;
    switch (renderParameters.depthStencilBits) {
        case DEPTH_STENCIL_BITS_D15S1:      depthStencilFormat = D3DFMT_D15S1; break;
        case DEPTH_STENCIL_BITS_D16:        depthStencilFormat = D3DFMT_D16; break;
        case DEPTH_STENCIL_BITS_D24S8:      depthStencilFormat = D3DFMT_D24S8; break;
        case DEPTH_STENCIL_BITS_D24X4S4:    depthStencilFormat = D3DFMT_D24X4S4; break;
        case DEPTH_STENCIL_BITS_D24X8:      depthStencilFormat = D3DFMT_D24X8; break;
        case DEPTH_STENCIL_BITS_D32:        depthStencilFormat = D3DFMT_D32; break;
    }

    D3DPRESENT_PARAMETERS presentParameters;
    ZeroMemory(&presentParameters, sizeof(presentParameters));
    presentParameters.BackBufferCount = 1;
    presentParameters.BackBufferFormat = format;
    presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParameters.MultiSampleQuality = 0;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.hDeviceWindow = hWnd;
    presentParameters.Flags = 0;
    presentParameters.FullScreen_RefreshRateInHz = renderParameters.refreshRate;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = depthStencilFormat;

    if (window.IsWindowed()) {
        presentParameters.Windowed = TRUE;
    } else {
        presentParameters.Windowed = FALSE;
        presentParameters.BackBufferWidth = renderParameters.width;
        presentParameters.BackBufferHeight = renderParameters.height;
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

void RenderContextDirect3D9::QueryAdapterSupportedDisplayFormats() {
    D3DFORMAT formats[] = {
        D3DFMT_A1R5G5B5,
        D3DFMT_A2R10G10B10,
        D3DFMT_A8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_X8R8G8B8
    };
    size_t numberFormats = sizeof(formats) / sizeof(D3DFORMAT);

    for (size_t i = 0; i < numberFormats; i++) {
        size_t numberModes = direct3D9Context_->GetAdapterModeCount(0, formats[i]);
        D3DDISPLAYMODE mode;

        for (size_t j = 0; j < numberModes; j++) {
            if (SUCCEEDED(direct3D9Context_->EnumAdapterModes(D3DADAPTER_DEFAULT, formats[i], j, &mode))) {
                DisplayMode_t displayMode;
                displayMode.width = mode.Width;
                displayMode.height = mode.Height;
                displayMode.refreshRate = mode.RefreshRate;

                DisplayFormat_t displayFormat;
                switch (mode.Format) {
                    case D3DFMT_A1R5G5B5:       displayFormat = DISPLAY_FORMAT_A1R5G5B5; break;
                    case D3DFMT_A2R10G10B10:    displayFormat = DISPLAY_FORMAT_A2R10G10B10; break;
                    case D3DFMT_A8R8G8B8:       displayFormat = DISPLAY_FORMAT_A8R8G8B8; break;
                    case D3DFMT_R5G6B5:         displayFormat = DISPLAY_FORMAT_R5G6B5; break;
                    case D3DFMT_X1R5G5B5:       displayFormat = DISPLAY_FORMAT_X1R5G5B5; break;
                    case D3DFMT_X8R8G8B8:       displayFormat = DISPLAY_FORMAT_X8R8G8B8; break;
                }
                displayMode.displayFormat = displayFormat;

                supportedDisplayModes_.push_back(displayMode);
            }
        }
    }
}

}