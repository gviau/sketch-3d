#include "render/OpenGL/RenderTextureOpenGL.h"

#include "render/OpenGL/Texture2DOpenGL.h"
#include "system/Logger.h"

#include "render/OpenGL/gl/glew.h"


namespace Sketch3D {

RenderTextureOpenGL::RenderTextureOpenGL(unsigned int width, unsigned int height, TextureFormat_t format, Texture2DOpenGL* texture) :
        RenderTexture(width, height, format), framebuffer_(0), renderbuffer_(0), texture_(texture), isValidForCreation_(true)
{
    if (texture_ != nullptr && texture_->width_ != width_ || texture_->height_ != height_ || texture_->format_ != format_) {
        isValidForCreation_ = false;
    }
}

RenderTextureOpenGL::~RenderTextureOpenGL() {
    if (renderbuffer_ != 0) {
        glDeleteRenderbuffers(1, &renderbuffer_);
    }

    if (framebuffer_ != 0) {
        glDeleteFramebuffers(1, &framebuffer_);
    }
}

bool RenderTextureOpenGL::AddDepthBuffer() {
    if (!isValidForCreation_) {
        return false;
    }
    
    if (framebuffer_ == 0) {
        glGenFramebuffers(1, &framebuffer_);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glGenRenderbuffers(1, &renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool RenderTextureOpenGL::Create() {
    if (!isValidForCreation_) {
        return false;
    }

    if (framebuffer_ == 0) {
        glGenFramebuffers(1, &framebuffer_);
    }

    if (texture_ != nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_->textureName_, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            Logger::GetInstance()->Info("Render texture #" + to_string(framebuffer_) + " was succesfully created");
            return true;
        }

        glDeleteFramebuffers(1, &framebuffer_);
    }

    Logger::GetInstance()->Error("Couldn't create render texture");

    return false;
}

void RenderTextureOpenGL::Bind() const {
    if (framebuffer_ == 0 && renderbuffer_ == 0) {
        return;
    }

    if (renderbuffer_ != 0 && framebuffer_ != 0) {
        //glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    } else if (framebuffer_ != 0) {
        //texture_->Bind(1);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
        //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    } else if (renderbuffer_ != 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    }
}

void RenderTextureOpenGL::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

}