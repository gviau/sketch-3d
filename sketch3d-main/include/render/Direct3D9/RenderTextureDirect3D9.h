#ifndef SKETCH_3D_RENDER_TEXTURE_DIRECT3D9_H
#define SKETCH_3D_RENDER_TEXTURE_DIRECT3D9_H

#include "render/RenderTexture.h"

#include <vector>
using namespace std;

// Forward class declaration
struct IDirect3DDevice9;
struct IDirect3DSurface9;

namespace Sketch3D {

/**
 * @class RenderTextureDirect3D9
 * Direct3D9 implementation of a render texture
 */
class RenderTextureDirect3D9  : public RenderTexture {
    public:
        /**
         * Constructor
         * @param device A pointer to the Direct3D9 device
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         */
                                    RenderTextureDirect3D9(IDirect3DDevice9* device, unsigned int width, unsigned int height, TextureFormat_t format);

        /**
         * Destructor
         */
        virtual                    ~RenderTextureDirect3D9();

        /**
         * Add a depthbuffer to the current render texture.
         * @return false if there is already a texture bound to the depth buffer or if there was an error, true otherwise
         */
        virtual bool                AddDepthBuffer();

        /**
         * Create a texture to be used with this RenderTexture
         * @return A pointer to a valid texture that can be used with this RenderTexture
         */
        virtual Texture2D*          CreateTexture2D() const;

        /**
         * Create a texture to be used as a depth buffer with this RenderTexture
         * @return A pointer to a valid texture that can be used as a depth buffer with this RenderTexture
         */
        virtual Texture2D*          CreateDepthBufferTexture() const;

        /**
         * Attach a texture to the depth buffer.
         * @param texture The texture to attach
         * @return false if there is already a depth buffer attached or if there was an error, true otherwise
         */
        virtual bool                AttachTextureToDepthBuffer(Texture2D* texture);

        /**
         * Create the render texture
         * @param textures The receiving color textures to use for this render texture. Must be of the same size as
         * the render texture
         * @return true if the render texture could be created, false otherwise
         */
        virtual bool                AttachTextures(const vector<Texture2D*>& textures);

        /**
         * Bind the render texture, that is, use it for the current rendering. This function will resize the
         * viewport to match the render texture width and height.
         */
        virtual void                Bind() const;

    private:
        IDirect3DDevice9*           device_;    /**< Direct3D9 device */
        IDirect3DSurface9*          depthBufferSurface_;    /**< Surface used to render in depth buffer */
        vector<IDirect3DSurface9*>  attachedSurfaces_;  /**< List of attached textures' surface */

        static int                  numGeneratedTextures_;
};

}

#endif