#ifndef SKETCH_3D_RENDER_CONTEXT_DIRECT3D11_H
#define SKETCH_3D_RENDER_CONTEXT_DIRECT3D11_H

#include "render/RenderContext.h"

#include <Windows.h>

// Forward class declaration
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct IDXGISwapChain;

namespace Sketch3D {

/**
 * @class RenderContextDirect3D11
 * Direct3D11 implementation of the render context
 */
class SKETCH_3D_API RenderContextDirect3D11 : public RenderContext {
    friend class RenderDeviceDirect3D11;

    public:
        /**
         * Constructor
         */
                                    RenderContextDirect3D11();

        /**
         * Destructor. Free the render context
         */
        virtual                    ~RenderContextDirect3D11();

        virtual bool                Initialize(Window& window, const RenderParameters_t& renderParameters) override;
        virtual void                SwapBuffers() override;

        /**
         * Returns the Direct3D11 device
         */
        ID3D11Device*               GetDevice() const;

        /**
         * Returns the Direct3D11 device context
         */
        ID3D11DeviceContext*        GetDeviceContext() const;

    private:
        ID3D11Device*               device_;
        IDXGISwapChain*             swapChain_;
        ID3D11DeviceContext*        context_;

        virtual void                QueryAdapterSupportedDisplayFormats() override;
};
}

#endif