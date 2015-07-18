#include "render/Direct3D11/RenderContextDirect3D11.h"

#include "system/Logger.h"
#include "system/Window.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
RenderContextDirect3D11::RenderContextDirect3D11() : context_(nullptr), swapChain_(nullptr), device_(nullptr), backbuffer_(nullptr), depthStencil_(nullptr), depthStencilBuffer_(nullptr) {
}

RenderContextDirect3D11::~RenderContextDirect3D11() {
    if (swapChain_ != nullptr) {
        swapChain_->Release();
    }

    if (backbuffer_ != nullptr) {
        backbuffer_->Release();
    }

    if (depthStencil_ != nullptr) {
        depthStencil_->Release();
    }
    
    if (depthStencilBuffer_ != nullptr) {
        depthStencilBuffer_->Release();
    }

    if (device_ != nullptr) {
        device_->Release();
    }

    if (context_ != nullptr) {
        context_->Release();
    }
}

bool RenderContextDirect3D11::Initialize(Window& window, const RenderParameters_t& renderParameters) {
    Logger::GetInstance()->Debug("Initializing Direct3D11 context");

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

    // Create the default back buffer
    ID3D11Texture2D* backbuffer;
    result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
    result = device_->CreateRenderTargetView(backbuffer, nullptr, &backbuffer_);
    backbuffer->Release();

    // Create the default depth stencil buffer
    DXGI_FORMAT depthStencilFormat;
    switch (renderParameters.depthStencilBits) {
        case DepthStencilBits_t::D16:   depthStencilFormat = DXGI_FORMAT_D16_UNORM; break;
        case DepthStencilBits_t::D24S8: depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
        case DepthStencilBits_t::D32:   depthStencilFormat = DXGI_FORMAT_D32_FLOAT; break;
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = renderParameters.width;
    depthStencilDesc.Height = renderParameters.height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = depthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    result = device_->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil_);
    result = device_->CreateDepthStencilView(depthStencil_, nullptr, &depthStencilBuffer_);

    // Set the default rasterizer state
    D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    ID3D11RasterizerState* rasterizerState;
    result = device_->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    context_->RSSetState(rasterizerState);

    // Set the default viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)renderParameters.width;
    viewport.Height = (float)renderParameters.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context_->RSSetViewports(1, &viewport);

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

ID3D11RenderTargetView* RenderContextDirect3D11::GetBackBuffer() const {
    return backbuffer_;
}

ID3D11DepthStencilView* RenderContextDirect3D11::GetDepthStencilBuffer() const {
    return depthStencilBuffer_;
}

void RenderContextDirect3D11::QueryAdapterSupportedDisplayFormats() {
}

}