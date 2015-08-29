#ifndef SKETCH_3D_ERROR_CHECKING_OPENGL_H
#define SKETCH_3D_ERROR_CHECKING_OPENGL_H

namespace Sketch3D {

void CheckOpenGLError(const char* stmt, const char* fname, int line);

#ifdef _DEBUG
    #define GL_CALL(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
    #define GL_CALL(stmt) stmt
#endif
}

#endif