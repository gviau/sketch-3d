#ifndef SKETCH_3D_TEXTURE_OPENGL_H
#define SKETCH_3D_TEXTURE_OPENGL_H

#include "render/Texture.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

#include <memory>
using namespace std;

namespace Sketch3D {

// Forward class declaration
class SamplerState;

class SKETCH_3D_API TextureOpenGL {
    public:
                        TextureOpenGL();
        virtual        ~TextureOpenGL();

        virtual void    ApplySamplerState(SamplerState* samplerState) = 0;
        virtual void    Bind() = 0;
        GLuint          GetTexture() const;

    protected:
        GLuint          texture_;
};

class SKETCH_3D_API Texture1DOpenGL : public Texture1D, public TextureOpenGL {
    public:
        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        virtual void        ApplySamplerState(SamplerState* samplerState) override;
        virtual void        Bind() override;
};

class SKETCH_3D_API Texture2DOpenGL : public Texture2D, public TextureOpenGL {
    public:
        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        virtual void        ApplySamplerState(SamplerState* samplerState) override;
        virtual void        Bind() override;
};

class SKETCH_3D_API Texture3DOpenGL : public Texture3D, public TextureOpenGL {
    public:
        virtual bool        Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable=false) override;
        virtual void        ApplySamplerState(SamplerState* samplerState) override;
        virtual void        Bind() override;
};

void GetOGLTextureFormat(TextureFormat_t textureFormat, GLuint& internalFormat, GLuint& components, GLuint& type, GLuint& bytesPerPixel);

}

#endif