#include "render/OpenGL/ErrorCheckingOpenGL.h"

#include "render/OpenGL/GL/glew.h"
#include "render/OpenGL/GL/gl.h"

#include "system/Logger.h"

namespace Sketch3D {

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        char buffer[1024];
        sprintf(buffer, "OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        Logger::GetInstance()->Error(buffer);
        Logger::GetInstance()->Error("    " + string((const char*)glewGetErrorString(err)));

        err = glGetError();
    }
}

}