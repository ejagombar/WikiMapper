#include "./selector.hpp"
#include "../logger.hpp"

SelectorSystem::SelectorSystem(int width, int height) : m_width(width), m_height(height) {

    globalLogger->info("Initialising Picking System");

    InitFramebuffer(width, height);
}

SelectorSystem::~SelectorSystem() {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_colorTexture);
    glDeleteRenderbuffers(1, &m_depthRenderBuffer);
}

void SelectorSystem::InitFramebuffer(int width, int height) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        globalLogger->error("Framebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SelectorSystem::Resize(int width, int height) {
    m_width = width;
    m_height = height;

    glDeleteTextures(1, &m_colorTexture);
    glDeleteRenderbuffers(1, &m_depthRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SelectorSystem::Begin() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
}

void SelectorSystem::End() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

// int SelectorSystem::ReadNodeID(int x, int y) {
//     if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
//         return -1;
//     }
//
//     glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//
//     GLubyte pixel[3];
//     glReadPixels(x, m_height - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
//
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//     int nodeID = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);
//
//     if (nodeID == 0) {
//         return -1;
//     }
//
//     return nodeID - 1;
// }
int SelectorSystem::ReadNodeID(int x, int y) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    GLubyte pixel[3];
    glReadPixels(x, m_height - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Make sure this matches how your shader encodes the ID
    int nodeID = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);

    // Only return ID if it's not the background (0)
    if (nodeID == 0) {
        return -1;
    }

    return nodeID - 1;
}
