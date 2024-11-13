#include "filter.hpp"

#include <GL/gl.h>
#include <stdexcept>

namespace Filter {

Blur::Blur(Shader &blurShader, GLuint screenWidth, GLuint screenHeight, bool enabled)
    : m_enabled(enabled), m_blurShader(blurShader), m_screenWidth(screenWidth), m_screenHeight(screenHeight) {
    // Two triangles that will cover the full screen when rendered in screen space. Position and texture coordinates.
    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    glGenBuffers(1, &m_quadVBO);
    glGenVertexArrays(1, &m_quadVAO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    // Position Coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    // Texture Coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // Framebuffers and textures for blur passes
    glGenFramebuffers(2, m_blurFBO);
    glGenTextures(2, m_blurTexture);
    glGenRenderbuffers(2, m_rboDepth);
    initSizeDependantBuffers();
}

void Blur::initSizeDependantBuffers() {
    for (GLuint i = 0; i < sizeof(m_blurTexture) / sizeof(m_blurTexture[0]); i++) {
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);

        // Bind the Texture attachment to the frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture[i], 0);

        glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screenWidth, m_screenHeight);

        // Bind the Renderbuffer attachment to the frame buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth[i]);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("FILTER::FRAMEBUFFER:: Blur framebuffer is not complete!");
    }

    // Make the default framebuffer active again to ensure that further render operations are displayed to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Blur::~Blur() {
    glDeleteFramebuffers(2, m_blurFBO);
    glDeleteTextures(2, m_blurTexture);
    glDeleteBuffers(2, m_rboDepth);

    glDeleteBuffers(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

void Blur::Preprocess() {
    if (!m_enabled) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[0]);
}

void Blur::Resize(const GLuint screenWidth, const GLuint screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    initSizeDependantBuffers();
}

void Blur::Display() {
    if (!m_enabled) {
        return;
    }

    m_blurShader.use();
    m_blurShader.setInt("boarder", m_boarder);
    m_blurShader.setInt("radius", m_radius);
    m_blurShader.setFloat("blurScale", m_blurScale);
    m_blurShader.setFloat("brightnessModifier", m_brightnessModifier);

    bool horizontal(true);
    for (uint i = 0; i < m_numBlurPasses; i++) {
        horizontal = ((i ^ 1) == (i + 1));
        m_blurShader.setBool("horizontal", horizontal);
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[horizontal ? 1 : 0]);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(m_quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[horizontal ? 0 : 1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    horizontal = !horizontal;
    m_blurShader.setBool("horizontal", horizontal);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(m_quadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[horizontal ? 0 : 1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace Filter
