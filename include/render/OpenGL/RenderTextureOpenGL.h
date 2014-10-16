#ifndef SKETCH_3D_RENDER_TEXTURE_OPENGL_H
#define SKETCH_3D_RENDER_TEXTURE_OPENGL_H

#include "render/RenderTexture.h"

#include <vector>
using namespace std;

namespace Sketch3D {

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
         */
                            RenderTextureOpenGL(unsigned int width, unsigned int height, TextureFormat_t format);

        /**
         * Destructor
         */
        virtual            ~RenderTextureOpenGL();

        /**
         * Add a depthbuffer to the current render texture.
         * @return false if there is already a texture bound to the depth buffer or if there was an error, true otherwise
         */
        virtual bool        AddDepthBuffer();

        /**
         * Attach a texture to the depth buffer.
         * @param texture The texture to attach
         * @return false if there is already a depth buffer attached or if there was an error, true otherwise
         */
        virtual bool        AttachTextureToDepthBuffer(Texture2D* texture);

        /**
         * Create the render texture
         * @param textures The receiving color textures to use for this render texture. Must be of the same size as
         * the render texture
         * @return true if the render texture could be created, false otherwise
         */
        virtual bool        AttachTextures(const vector<Texture2D*>& textures);

        /**
         * Bind the render texture, that is, use it for the current rendering. This function will resize the
         * viewport to match the render texture width and height.
         */
        virtual void        Bind() const;

    private:
        size_t              framebuffer_; /**< OpenGL's name of the framebuffer */
        size_t              renderbuffer_;    /**< OpenGL's name of the renderbuffer */
        vector<Texture2D*>  textures_;   /**< The textures used to receive the output of the rendering */
        bool                depthBufferBound_;  /**< Used to determine if there is a texture attached to the depth buffer */
        bool                texturesAttached_;  /**< Used to determine if the textures were correctly attached to the frame buffer */
};

}

#endif