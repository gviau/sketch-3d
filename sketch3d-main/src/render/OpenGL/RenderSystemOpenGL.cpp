#include "render/OpenGL/RenderSystemOpenGL.h"

#include "render/OpenGL/gl/glew.h"
#include "render/OpenGL/gl/gl.h"

#include "render/OpenGL/BufferObjectManagerOpenGL.h"
#include "render/OpenGL/BufferObjectOpenGL.h"
#include "render/OpenGL/RenderTextureOpenGL.h"
#include "render/OpenGL/ShaderOpenGL.h"
#include "render/OpenGL/Texture2DOpenGL.h"
#include "render/OpenGL/Texture3DOpenGL.h"

#include "system/Logger.h"
#include "system/Window.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"
#endif

namespace Sketch3D {

RenderSystemOpenGL::RenderSystemOpenGL(Window& window) : RenderSystem(window), renderContext_(NULL) {
	Logger::GetInstance()->Info("Current rendering API: OpenGL");
}

RenderSystemOpenGL::~RenderSystemOpenGL() {
    TextureUnitNode_t* node = head_;
    TextureUnitNode_t* toDelete = nullptr;
    while (node != nullptr) {
        toDelete = node;
        node = node->next;
        delete toDelete;
    }

	Logger::GetInstance()->Info("Shutdown OpenGL");
    FreeRenderSystem();
	delete renderContext_;
}

bool RenderSystemOpenGL::Initialize(const RenderParameters_t& renderParameters) {
	Logger::GetInstance()->Info("Initializing OpenGL...");

#if PLATFORM == PLATFORM_WIN32
	renderContext_ = new RenderContextOpenGLWin32();
#elif PLATFORM == PLATFORM_LINUX
    renderContext_ = new RenderContextOpenGLUnix();
#endif

    if (!renderContext_->Initialize(window_, renderParameters)) {
		Logger::GetInstance()->Error("Couldn't create OpenGL context");
		return false;
	}

	QueryDeviceCapabilities();

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    bufferObjectManager_ = new BufferObjectManagerOpenGL;

    // Construct the texture cache
    head_ = new TextureUnitNode_t;
    head_->textureUnit = 0;
    head_->prev = nullptr;
    tail_ = head_;

    for (int i = 1; i < deviceCapabilities_.maxActiveTextures_ + 1; i++) {
        tail_->next = new TextureUnitNode_t;
        tail_->next->prev = tail_;
        tail_ = tail_->next;
        tail_->textureUnit = i;
    }
    tail_->next = nullptr;

    CreateTextShader();

	return true;
}

void RenderSystemOpenGL::SetClearColor(float red, float green, float blue, float alpha) {
	glClearColor(red, green, blue, alpha);
}

void RenderSystemOpenGL::Clear(int buffer) const {
    GLuint clearBuffer = 0;
    if ((buffer & CLEAR_BUFFER_COLOR) != 0) {
        clearBuffer |= GL_COLOR_BUFFER_BIT;
    }

    if ((buffer & CLEAR_BUFFER_DEPTH) != 0) {
        clearBuffer |= GL_DEPTH_BUFFER_BIT;
    }

	glClear(clearBuffer);
}

void RenderSystemOpenGL::StartRender() {
}

void RenderSystemOpenGL::EndRender() {
}

void RenderSystemOpenGL::PresentFrame() {
    renderContext_->SwapBuffers();
}

Matrix4x4 RenderSystemOpenGL::OrthoProjection(float left, float right, float bottom, float top,
                                              float nearPlane, float farPlane) const
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = farPlane - nearPlane;

	Matrix4x4 projection;
	projection[0][0] = 2.0f / dx;
	projection[1][1] = 2.0f / dy;
	projection[2][2] = -2.0f / dz;
	projection[0][3] = -(right + left) / dx;
	projection[1][3] = -(top + bottom) / dy;
	projection[2][3] = -(farPlane + nearPlane) / dz;

    return projection;
}

Matrix4x4 RenderSystemOpenGL::PerspectiveProjection(float left, float right, float bottom, float top,
                                                    float nearPlane, float farPlane) const
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = nearPlane - farPlane;

	Matrix4x4 projection;
	projection[0][0] = 2.0f * nearPlane / dx;
	projection[1][1] = 2.0f * nearPlane / dy;
	projection[0][2] = (right + left) / dx;
	projection[1][2] = (top + bottom) / dy;
	projection[2][2] = (farPlane + nearPlane) / dz;
	projection[3][2] = -1.0f;
	projection[2][3] = 2.0f * nearPlane * farPlane / dz;
	projection[3][3] = 0.0f;

    return projection;
}

void RenderSystemOpenGL::SetRenderFillMode(RenderMode_t mode) const {
	switch (mode) {
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

void RenderSystemOpenGL::SetViewport(size_t x, size_t y, size_t width, size_t height) {
    glViewport(x, y, width, height);
}

void RenderSystemOpenGL::EnableDepthTest(bool val) const {
    if (val) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void RenderSystemOpenGL::EnableDepthWrite(bool val) const {
    glDepthMask( (val) ? GL_TRUE : GL_FALSE );
}

void RenderSystemOpenGL::EnableColorWrite(bool val) const {
    if (val) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    } else {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
}

void RenderSystemOpenGL::SetDepthComparisonFunc(DepthFunc_t comparison) const {
    GLenum depthFunc;
    switch (comparison) {
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

void RenderSystemOpenGL::SetCullingMethod(CullingMethod_t cullingMethod) const {
    switch (cullingMethod) {
        case CULLING_METHOD_FRONT_FACE:
            glCullFace(GL_FRONT);
            break;
        case CULLING_METHOD_BACK_FACE:
            glCullFace(GL_BACK);
            break;
    }
}

Shader* RenderSystemOpenGL::CreateShader() {
    shaders_.push_back(new ShaderOpenGL);
    return shaders_[shaders_.size() - 1];
}

Texture2D* RenderSystemOpenGL::CreateTexture2D() const {
    return new Texture2DOpenGL;
}

Texture3D* RenderSystemOpenGL::CreateTexture3D() const {
    return new Texture3DOpenGL;
}

RenderTexture* RenderSystemOpenGL::CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) {
    renderTextures_.push_back(new RenderTextureOpenGL(width, height, format));
    return renderTextures_.back();
}

void RenderSystemOpenGL::BindScreenBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Renderer::GetInstance()->SetViewport(0, 0, width_, height_);
}

void RenderSystemOpenGL::EnableBlending(bool val) const {
    if (val) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void RenderSystemOpenGL::SetBlendingEquation(BlendingEquation_t equation) const {
    GLenum blendingEquation;
    switch (equation) {
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

void RenderSystemOpenGL::SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) const {
    glBlendFunc(GetBlendingFactor(srcFactor), GetBlendingFactor(dstFactor));
}

size_t RenderSystemOpenGL::BindTexture(const Texture* texture) {
    size_t textureName;
    GLint textureType;
    if (texture->GetType() == TEXTURE_TYPE_2D) {
        textureName = static_cast<const Texture2DOpenGL*>(texture)->textureName_;
        textureType = GL_TEXTURE_2D;
    } else {
        textureName = static_cast<const Texture3DOpenGL*>(texture)->textureName_;
        textureType = GL_TEXTURE_3D;
    }

    // Verify if the node is already bound
    size_t textureUnit = 0;
    TextureUnitNode_t* nodeToUse = head_;
    TextureCache_t::iterator it = textureCache_.find(textureName);

    if (it != textureCache_.end()) {
        nodeToUse = it->second;
    } else {
        glActiveTexture(GL_TEXTURE0 + nodeToUse->textureUnit);
        glBindTexture(textureType, textureName);

        textureCache_[textureName] = nodeToUse;
    }
    textureUnit = nodeToUse->textureUnit;

    // Put the node to use at the end of the list if it isn't there yet
    if (tail_ != nodeToUse) {
        tail_->next = nodeToUse;

        // Remove that node from the list since its now a duplicate
        if (nodeToUse->prev) {
            nodeToUse->prev->next = nodeToUse->next;
        }

        if (nodeToUse->next) {
            nodeToUse->next->prev = nodeToUse->prev;
        }

        // Advance head if needed
        if (nodeToUse == head_) {
            head_ = head_->next;
        }

        tail_->next->prev = tail_;
        tail_ = tail_->next;
        tail_->next = nullptr;
    }

    return textureUnit;
}

void RenderSystemOpenGL::BindShader(const Shader* shader) {
    if (shader != boundShader_) {
        if (shader == nullptr) {
            glUseProgram(0);
        } else {
            const ShaderOpenGL* shaderOpengl = static_cast<const ShaderOpenGL*>(shader);
            glUseProgram(shaderOpengl->program_);
        }

        boundShader_ = shader;
    }
}

FrustumPlanes_t RenderSystemOpenGL::ExtractViewFrustumPlanes(const Matrix4x4& viewProjection) const {
    FrustumPlanes_t frustumPlanes;

    Vector3 nearPlaneNormal(viewProjection[3][0] + viewProjection[2][0], viewProjection[3][1] + viewProjection[2][1], viewProjection[3][2] + viewProjection[2][2]);
    float nearPlaneLength = nearPlaneNormal.Length();
    frustumPlanes.nearPlane.SetNormalizedNormal(nearPlaneNormal / nearPlaneLength);
    frustumPlanes.nearPlane.SetDistance( (viewProjection[3][3] + viewProjection[2][3]) / nearPlaneLength );

    Vector3 farPlaneNormal(viewProjection[3][0] - viewProjection[2][0], viewProjection[3][1] - viewProjection[2][1], viewProjection[3][2] - viewProjection[2][2]);
    float farPlaneLength = farPlaneNormal.Length();
    frustumPlanes.farPlane.SetNormalizedNormal(farPlaneNormal / farPlaneLength);
    frustumPlanes.farPlane.SetDistance( (viewProjection[3][3] - viewProjection[2][3]) / farPlaneLength );

    Vector3 leftPlaneNormal(viewProjection[3][0] + viewProjection[0][0], viewProjection[3][1] + viewProjection[0][1], viewProjection[3][2] + viewProjection[0][2]);
    float leftPlaneLength = leftPlaneNormal.Length();
    frustumPlanes.leftPlane.SetNormalizedNormal(leftPlaneNormal / leftPlaneLength);
    frustumPlanes.leftPlane.SetDistance( (viewProjection[3][3] + viewProjection[0][3]) / leftPlaneLength );

    Vector3 rightPlaneNormal(viewProjection[3][0] - viewProjection[0][0], viewProjection[3][1] - viewProjection[0][1], viewProjection[3][2] - viewProjection[0][2]);
    float rightPlaneLength = rightPlaneNormal.Length();
    frustumPlanes.rightPlane.SetNormalizedNormal(rightPlaneNormal / rightPlaneLength);
    frustumPlanes.rightPlane.SetDistance( (viewProjection[3][3] - viewProjection[0][3]) / rightPlaneLength );

    Vector3 bottomPlaneNormal(viewProjection[3][0] + viewProjection[1][0], viewProjection[3][1] + viewProjection[1][1], viewProjection[3][2] + viewProjection[1][2]);
    float bottomPlaneLength = bottomPlaneNormal.Length();
    frustumPlanes.bottomPlane.SetNormalizedNormal(bottomPlaneNormal / bottomPlaneLength);
    frustumPlanes.bottomPlane.SetDistance( (viewProjection[3][3] + viewProjection[1][3]) / bottomPlaneLength );

    Vector3 topPlaneNormal(viewProjection[3][0] - viewProjection[1][0], viewProjection[3][1] - viewProjection[1][1], viewProjection[3][2] - viewProjection[1][2]);
    float topPlaneLength = topPlaneNormal.Length();
    frustumPlanes.topPlane.SetNormalizedNormal(topPlaneNormal / topPlaneLength);
    frustumPlanes.topPlane.SetDistance( (viewProjection[3][3] - viewProjection[1][3]) / topPlaneLength );

    return frustumPlanes;
}

void RenderSystemOpenGL::QueryDeviceCapabilities() {
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &deviceCapabilities_.maxActiveTextures_);
}

unsigned int RenderSystemOpenGL::GetBlendingFactor(BlendingFactor_t factor) const {
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

void RenderSystemOpenGL::CreateTextShader() {
    const char* textVertexShaderSource = \
        "#version 330\n"

        "layout (location=0) in vec3 in_vertex;\n"
        "layout (location=1) in vec2 in_uv;\n"

        "out vec2 uv;\n"

        "void main() {\n"
            "uv = in_uv;\n"
            "gl_Position = vec4(in_vertex.xy, 0.0, 1.0);\n"
        "}"
    ;

    const char* textFragmentShaderSource = \
        "#version 330\n"

        "uniform sampler2D fontAtlas;\n"
        "uniform vec3 textColor;\n"

        "in vec2 uv;\n"
        "out vec4 color;\n"

        "void main() {\n"
            "float val = texture2D(fontAtlas, uv).r;\n"
            "if (val < 0.5) {\n"
                "discard;\n"
            "}\n"

            "color = vec4(textColor, 1.0);\n"
        "}"
    ;

    textShader_ = Renderer::GetInstance()->CreateShader();
    if (!textShader_->SetSource(textVertexShaderSource, textFragmentShaderSource)) {
        Logger::GetInstance()->Error("Couldn't create text shader");
    }
}

}
