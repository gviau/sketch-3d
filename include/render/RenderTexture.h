#ifndef SKETCH_3D_RENDER_TEXTURE_H
#define SKETCH_3D_RENDER_TEXTURE_H

#include "render/Texture2D.h"

namespace Sketch3D {

/**
 * @class RenderTexture
 * Interface for render textures. Render textures are used to output a fragment shader to a texture, instead of the
 * screen.
 */
class RenderTexture {
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
        virtual        ~RenderTexture();

        /**
         * Add a depthbuffer to the current render texture. There can only be one depthbuffer at once. Also, you should call
         * this function before creating the render texture
         * @return true if the depth buffer could be attached
         */
        virtual bool    AddDepthBuffer() = 0;

        /**
         * Create the render texture
         * @return true if the render texture could be created, false otherwise
         */
        virtual bool    Create() = 0;

        /**
         * Bind the render texture, that is, use it for the current rendering
         */
        virtual void    Bind() const = 0;

        /**
         * Unbind the render texture, that is, use the normal framebuffer (the screen) for rendering
         */
        virtual void    Unbind() const = 0;

    protected:
        size_t          width_; /**< The width of the render texture */
        size_t          height_;    /**< The height of the render texture */
        TextureFormat_t format_;    /**< The format of the render texture */
};

}

#endif