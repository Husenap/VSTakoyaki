#include "render_target.hpp"

#include <iostream>

namespace ht {

RenderTarget::RenderTarget(const glm::ivec2& size)
    : mSize(size) {
    Init();
}

void RenderTarget::Init() {
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    glGenTextures(1, &mRenderTexture);
    glBindTexture(GL_TEXTURE_2D, mRenderTexture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 mSize.x,
                 mSize.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRenderTexture, 0);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FAILED to create framebuffer!" << std::endl;
    }
}

void RenderTarget::Destroy() {
    glDeleteTextures(1, &mRenderTexture);
    glDeleteFramebuffers(1, &mFramebuffer);
}

RenderTarget::~RenderTarget() {
    Destroy();
}

void RenderTarget::Resize(const glm::ivec2& size) {
    Destroy();
    mSize = size;
    Init();
}

void RenderTarget::SetAsTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

uint8_t* RenderTarget::GetPixels() {
    uint8_t* pixels = new uint8_t[mSize.x * mSize.y * 3];

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mRenderTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    return pixels;
}

}  // namespace ht