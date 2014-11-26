#ifndef SKETCH_3D_BUFFER_OBJECT_OPENGL_H
#define SKETCH_3D_BUFFER_OBJECT_OPENGL_H

#include "render/BufferObject.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

namespace Sketch3D {

/**
 * @class BufferObjectOpenGL
 * 
 */
class BufferObjectOpenGL : public BufferObject {
    public:
                        BufferObjectOpenGL(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage=BUFFER_USAGE_STATIC);
        virtual        ~BufferObjectOpenGL();
        virtual void    Render() const;
        virtual bool    SetVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual bool    AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual void    SetIndexData(unsigned short* indexData, size_t numIndex);
        virtual void    AppendIndexData(unsigned short* indexData, size_t numIndex);

    private:
        GLuint          vao_;   /**< Vertex array object */
        GLuint          vbo_;   /**< Vertex buffer object */
        GLuint          ibo_;   /**< infex buffer object */

        /**
         * Generate the buffers' name
         */
        void            GenerateBuffers();
};

}

#endif