#ifndef SKETCH_3D_RENDER_TEXTURE_H
#define SKETCH_3D_RENDER_TEXTURE_H

#include "render/Texture2D.h"

#include "system/Platform.h"

namespace Sketch3D {

/**
 * @class RenderTexture
 * Interface for render textures. Render textures are used to output a fragment shader to a texture, instead of the
 * screen.
 */
class SKETCH_3D_API RenderTexture {
    public:
        /**
         * Constructor
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         */
                            RenderTexture(unsigned int width, unsigned int height, TextureFormat_t format);

        /**
         * Destructor
         */
        virtual            ~RenderTexture();

        /**
         * Add a depthbuffer to the current render texture. There can only be one depthbuffer at once. Also, you should call
         * this function before creating the render texture
         * @return false if there is already a texture bound to the depth buffer or if there was an error, true otherwise
         */
        virtual bool        AddDepthBuffer() = 0;

        /**
         * Create a texture to be used with this RenderTexture
         * @return A pointer to a valid texture that can be used with this RenderTexture
         */
        virtual Texture2D*  CreateTexture2D() const = 0;

        /**
         * Create a texture to be used as a depth buffer with this RenderTexture
         * @return A pointer to a valid texture that can be used as a depth buffer with this RenderTexture
         */
        virtual Texture2D*  CreateDepthBufferTexture() const = 0;

        /**
         * Attach a texture to the depth buffer.
         * @param texture The texture to attach
         * @return false if there is already a depth buffer attached or if there was an error, true otherwise
         */
        virtual bool        AttachTextureToDepthBuffer(Texture2D* texture) = 0;

        /**
         * Create the render texture
         * @param textures The receiving color textures to use for this render texture. Must be of the same size as
         * the render texture
         * @return true if the render texture could be created, false otherwise
         */
        virtual bool        AttachTextures(const vector<Texture2D*>& textures) = 0;

        /**
         * Bind the render texture, that is, use it for the current rendering. This function will resize the
         * viewport to match the render texture width and height.
         */
        virtual void        Bind() const = 0;

    protected:
        size_t              width_; /**< The width of the render texture */
        size_t              height_;    /**< The height of the render texture */
        TextureFormat_t     format_;    /**< The format of the render texture */
        bool                depthBufferBound_;  /**< Used to determine if there is a texture attached to the depth buffer */
        bool                texturesAttached_;  /**< Used to determine if the textures were correctly attached to the frame buffer */
};

}

#endif