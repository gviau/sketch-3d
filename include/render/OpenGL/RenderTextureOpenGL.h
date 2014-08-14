#ifndef SKETCH_3D_RENDER_TEXTURE_OPENGL_H
#define SKETCH_3D_RENDER_TEXTURE_OPENGL_H

#include "render/RenderTexture.h"

namespace Sketch3D {

// Forward declaration
class Texture2DOpenGL;

/**
 * @class RenderTextureOpenGL
 * Implements a render texture using the OpenGL API
 */
class RenderTextureOpenGL : public RenderTexture {
    public:
        /**
         * Constructor
         * @param width The width of the render texture
         * @param height The height of the render texture
         * @param format The format of the render texture
         * @param texture The receiving color texture to use for this render texture. Must be of the same size as
         * the render texture
         */
                        RenderTextureOpenGL(unsigned int width, unsigned int height, TextureFormat_t format, Texture2DOpenGL* texture);

        /**
         * Destructor
         */
        virtual        ~RenderTextureOpenGL();

        /**
         * Add a depthbuffer to the current render texture.
         * @return true if the depth buffer could be attached
         */
        virtual bool    AddDepthBuffer();

        /**
         * Create the render texture
         * @return true if the render texture could be created, false otherwise
         */
        virtual bool    Create();

        /**
         * Bind the render texture, that is, use it for the current rendering
         */
        virtual void    Bind() const;

        /**
         * Unbind the render texture, that is, use the normal framebuffer (the screen) for rendering
         */
        virtual void    Unbind() const;

    private:
        size_t              framebuffer_; /**< OpenGL's name of the framebuffer */
        size_t              renderbuffer_;    /**< OpenGL's name of the renderbuffer */
        Texture2DOpenGL*    texture_;   /**< The texture used to receive the output of the rendering */
        bool                isValidForCreation_;    /**< Is the texture passed in the constructor valid for creation of the render texture */
};

}

#endif