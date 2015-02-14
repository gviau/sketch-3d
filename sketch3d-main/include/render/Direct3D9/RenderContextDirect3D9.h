#ifndef SKETCH_3D_RENDER_CONTEXT_DIRECT3D9_H
#define SKETCH_3D_RENDER_CONTEXT_DIRECT3D9_H

#include "render/RenderContext.h"

#include <Windows.h>

// Forward class declaration
struct IDirect3D9;
struct IDirect3DDevice9;

namespace Sketch3D {

/**
 * @class RenderContextDirect3D9
 * Direct3D9 implementation of the render context
 */
class RenderContextDirect3D9 : public RenderContext {
    public:
        /**
         * Constructor
         */
                            RenderContextDirect3D9();

        /**
         * Destructor. Free the render context
         */
        virtual            ~RenderContextDirect3D9();

        virtual bool        Initialize(Window& window, const RenderParameters_t& renderParameters);
        virtual void        SwapBuffers();

        /**
         * Returns the Direct3D9 device
         */
        IDirect3DDevice9*   GetDevice() const;

    private:
        IDirect3D9*         direct3D9Context_;
        IDirect3DDevice9*   direct3D9Device_;

        virtual void        QueryAdapterSupportedDisplayFormats();
};

}

#endif