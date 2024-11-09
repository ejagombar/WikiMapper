#include "filter.hpp"

#include <stdexcept>

namespace Filter {

Blur::Blur(Shader &blurShader, uint screenWidth, uint screenHeight, bool enabled)
    : m_enabled(enabled), m_blurShader(blurShader), m_screenWidth(screenWidth), m_screenHeight(screenHeight) {

    // Quad vertices for a fullscreen quad
    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    unsigned int quadVBO;
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // Framebuffer for the original scene
    glGenFramebuffers(1, &m_sceneFBO);
    glGenTextures(1, &m_sceneTexture);

    // Framebuffers and textures for blur passes
    glGenFramebuffers(2, m_blurFBO);
    glGenTextures(2, m_blurTexture);
    initSizeDependantBuffers();
}

void Blur::initSizeDependantBuffers() {
    glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneTexture, 0);

    // Depth buffer for scene framebuffer
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screenWidth, m_screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("FILTER::FRAMEBUFFER:: Scene framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (unsigned int i = 0; i < sizeof(m_blurTexture) / sizeof(m_blurTexture[0]); i++) {
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("FILTER::FRAMEBUFFER:: Blur framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Blur::Preprocess() {
    if (!m_enabled) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
}

void Blur::Resize(const int screenWidth, const int screenHeight) {
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
    glBindVertexArray(m_quadVAO);
    for (int i = 0; i < m_numBlurPasses; i++) {
        for (unsigned int j = 0; j < 2; j++) {
            horizontal = (j == 0);
            m_blurShader.setBool("horizontal", horizontal);
            glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[j]);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindTexture(GL_TEXTURE_2D, (i == 0 && j == 0) ? m_sceneTexture : m_blurTexture[1 - j]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace Filter
