#ifndef SKETCH_3D_SHADER_OPENGL_H
#define SKETCH_3D_SHADER_OPENGL_H

#include "render/Shader.h"

#include "render/RenderDevice_Common.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {
// Class forward declaration
class VertexFormat;

class SKETCH_3D_API ShaderOpenGL {
    public:
                ShaderOpenGL();
               ~ShaderOpenGL();
    protected:
        GLuint  shader_;

        bool    ValidateShader() const;
};

class SKETCH_3D_API FragmentShaderOpenGL : public FragmentShader, public ShaderOpenGL {
    public:
        virtual bool        InitializeFromSource(const string& source) override;
};

class SKETCH_3D_API VertexShaderOpenGL : public VertexShader, public ShaderOpenGL {
    public:
        virtual bool        InitializeFromSource(const string& source) override;
        virtual bool        CreateInputLayout(VertexFormat* vertexFormat) override;
};  

}

#endif