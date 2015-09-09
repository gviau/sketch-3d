#include "render/Direct3D11/RenderContextDirect3D11.h"

#include "system/Logger.h"
#include "system/Window.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
RenderContextDirect3D11::RenderContextDirect3D11() : context_(nullptr), swapChain_(nullptr), device_(nullptr) {
}

RenderContextDirect3D11::~RenderContextDirect3D11() {
    if (swapChain_ != nullptr) {
        swapChain_->Release();
    }

    if (context_ != nullptr) {
        context_->Release();
    }

#if _DEBUG
    ID3D11Debug* debug = nullptr;
    device_->QueryInterface(IID_PPV_ARGS(&debug));
#endif

    if (device_ != nullptr) {
        device_->Release();
    }

#if _DEBUG
    if (debug != nullptr) {
        debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
#endif

    Logger::GetInstance()->Info("Direct3D context destroyed");
}

bool RenderContextDirect3D11::Initialize(Window& window, const RenderParameters_t& renderParameters) {
    Logger::GetInstance()->Debug("Initializing Direct3D11 context");
    renderParameters_ = renderParameters;

    DXGI_FORMAT backbufferFormat;
    switch (renderParameters.displayFormat) {
        case DisplayFormat_t::A1R5G5B5:     backbufferFormat = DXGI_FORMAT_B5G5R5A1_UNORM; break;
        case DisplayFormat_t::A2R10G10B10:  backbufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM; break;
        case DisplayFormat_t::A8R8G8B8:     backbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case DisplayFormat_t::R5G6B5:       backbufferFormat = DXGI_FORMAT_B5G6R5_UNORM; break;
        case DisplayFormat_t::X8R8G8B8:     backbufferFormat = DXGI_FORMAT_B8G8R8X8_UNORM; break;
    }

    HWND hWnd = reinterpret_cast<HWND>(window.GetHandle());
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = backbufferFormat;
    swapChainDesc.BufferDesc.Height = renderParameters.height;
    swapChainDesc.BufferDesc.Width = renderParameters.width;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = window.IsWindowed();

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT result;
#ifdef _DEBUG
    result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
                                           &swapChain_, &device_, &featureLevel, &context_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create render context and swap chain");
        return false;
    }
#else
    result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, nullptr, 0, D3D11_SDK_VERSION, &device_, &featureLevel, &context_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create the Direct3D11 context");
        return false;
    }

    IDXGIDevice1* dxgiDevice = nullptr;
    device_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);

    IDXGIAdapter* dxgiAdapter = nullptr;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    IDXGIFactory1* dxgiFactory = nullptr;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);

    result = dxgiFactory->CreateSwapChain(device_, &swapChainDesc, &swapChain_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create swap chain");
        return false;
    }

    dxgiDevice->Release();
    dxgiAdapter->Release();
    dxgiFactory->Release();
#endif

    string featureLevelStr = "";
    switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_0:
            featureLevelStr = "Direct3D11";
            break;

        case D3D_FEATURE_LEVEL_10_1:
        case D3D_FEATURE_LEVEL_10_0:
            featureLevelStr = "Direct3D10";
            break;

        case D3D_FEATURE_LEVEL_9_3:
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            featureLevelStr = "Direct3D9";
            break;
    }

    Logger::GetInstance()->Info("Direct3D11 context created");
    Logger::GetInstance()->Info("Feature Level = " + featureLevelStr);

    return true;
}

void RenderContextDirect3D11::SwapBuffers() {
    swapChain_->Present(0, 0);
}

ID3D11Device* RenderContextDirect3D11::GetDevice() const {
    return device_;
}

ID3D11DeviceContext* RenderContextDirect3D11::GetDeviceContext() const {
    return context_;
}

void RenderContextDirect3D11::QueryAdapterSupportedDisplayFormats() {
}

}