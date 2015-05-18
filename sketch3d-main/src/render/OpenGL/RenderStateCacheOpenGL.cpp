#include "render/OpenGL/RenderStateCacheOpenGL.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

namespace Sketch3D {
RenderStateCacheOpenGL::RenderStateCacheOpenGL() {
    EnableDepthTestImpl();
    EnableDepthWriteImpl();
    EnableColorWriteImpl();
    EnableBlendingImpl();
    SetDepthComparisonFuncImpl();
    SetCullingMethodImpl();
    SetBlendingEquationImpl();
    SetBlendingFactorImpl();
    SetRenderFillModeImpl();
}

RenderStateCacheOpenGL::~RenderStateCacheOpenGL() {

}

void RenderStateCacheOpenGL::EnableDepthTestImpl() {
    if (isDepthTestEnabled_) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void RenderStateCacheOpenGL::EnableDepthWriteImpl() {
    glDepthMask( (isDepthWriteEnabled_) ? GL_TRUE : GL_FALSE );
}

void RenderStateCacheOpenGL::EnableColorWriteImpl() {
    if (isColorWriteEnabled_) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    } else {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
}

void RenderStateCacheOpenGL::EnableBlendingImpl() {
    if (isBlendingEnabled_) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void RenderStateCacheOpenGL::SetDepthComparisonFuncImpl() {
    GLenum depthFunc;
    switch (depthComparisonFunction_) {
        case DEPTH_FUNC_NEVER:
            depthFunc = GL_NEVER;
            break;
        case DEPTH_FUNC_ALWAYS:
            depthFunc = GL_ALWAYS;
            break;
        case DEPTH_FUNC_LESS:
            depthFunc = GL_LESS;
            break;
        case DEPTH_FUNC_LEQUAL:
            depthFunc = GL_LEQUAL;
            break;
        case DEPTH_FUNC_EQUAL:
            depthFunc = GL_EQUAL;
            break;
        case DEPTH_FUNC_GEQUAL:
            depthFunc = GL_GEQUAL;
            break;
        case DEPTH_FUNC_GREATER:
            depthFunc = GL_GREATER;
            break;
        case DEPTH_FUNC_NOTEQUAL:
            depthFunc = GL_NOTEQUAL;
            break;
    }

    glDepthFunc(depthFunc);
}

void RenderStateCacheOpenGL::SetCullingMethodImpl() {
    switch (cullingMethod_) {
        case CULLING_METHOD_FRONT_FACE:
            glCullFace(GL_FRONT);
            break;
        case CULLING_METHOD_BACK_FACE:
            glCullFace(GL_BACK);
            break;
    }
}

void RenderStateCacheOpenGL::SetBlendingEquationImpl() {
    GLenum blendingEquation;
    switch (blendingEquation_) {
        case BLENDING_EQUATION_ADD:
            blendingEquation = GL_FUNC_ADD;
            break;
        case BLENDING_EQUATION_SUBTRACT:
            blendingEquation = GL_FUNC_SUBTRACT;
            break;
        case BLENDING_EQUATION_REVERSE_SUBTRACT:
            blendingEquation = GL_FUNC_REVERSE_SUBTRACT;
            break;
        case BLENDING_EQUATION_MIN:
            blendingEquation = GL_MIN;
            break;
        case BLENDING_EQUATION_MAX:
            blendingEquation = GL_MAX;
            break;
    }

    glBlendEquation(blendingEquation);
}

void RenderStateCacheOpenGL::SetBlendingFactorImpl() {
    glBlendFunc(GetBlendingFactor(sourceBlendingFactor_), GetBlendingFactor(destinationBlendingFactor_));
}

void RenderStateCacheOpenGL::SetRenderFillModeImpl() {
    switch (renderMode_) {
        case RENDER_MODE_FILL:
            glPolygonMode(GL_FRONT, GL_FILL);
            break;
        case RENDER_MODE_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case RENDER_MODE_POINT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }
}

unsigned int RenderStateCacheOpenGL::GetBlendingFactor(BlendingFactor_t factor) const {
    unsigned int blendingFactor;
    switch (factor) {
        case BLENDING_FACTOR_ZERO:
            blendingFactor = GL_ZERO;
            break;
        case BLENDING_FACTOR_ONE:
            blendingFactor = GL_ONE;
            break;
        case BLENDING_FACTOR_SRC_COLOR:
            blendingFactor = GL_SRC_COLOR;
            break;
        case BLENDING_FACTOR_ONE_MINUS_SRC_COLOR:
            blendingFactor = GL_ONE_MINUS_SRC_COLOR;
            break;
        case BLENDING_FACTOR_DST_COLOR:
            blendingFactor = GL_DST_COLOR;
            break;
        case BLENDING_FACTOR_ONE_MINUS_DST_COLOR:
            blendingFactor = GL_ONE_MINUS_DST_COLOR;
            break;
        case BLENDING_FACTOR_SRC_ALPHA:
            blendingFactor = GL_SRC_ALPHA;
            break;
        case BLENDING_FACTOR_ONE_MINUS_SRC_ALPHA:
            blendingFactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case BLENDING_FACTOR_DST_ALPHA:
            blendingFactor = GL_DST_ALPHA;
            break;
        case BLENDING_FACTOR_ONE_MINUS_DST_ALPHA:
            blendingFactor = GL_ONE_MINUS_DST_ALPHA;
            break;
        case BLENDING_FACTOR_SRC_ALPHA_SATURATE:
            blendingFactor = GL_SRC_ALPHA_SATURATE;
            break;
    }

    return blendingFactor;
}

}