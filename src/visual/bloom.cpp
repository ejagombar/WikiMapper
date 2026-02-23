#include "bloom.hpp"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <stdexcept>

namespace Filter {

Bloom::Bloom(glm::ivec2 screenSize, float threshold, float intensity, uint32_t passes, float blurScale, float softKnee)
    : m_screenSize(screenSize), m_halfSize(screenSize / 2), m_threshold(threshold), m_intensity(intensity),
      m_passes(passes), m_blurScale(blurScale), m_softKnee(softKnee) {

    m_extractShader = std::make_unique<Shader>("framebuffer.vert", "bloomExtract.frag");
    m_blurShader = std::make_unique<Shader>("framebuffer.vert", "bloomBlur.frag");
    m_compositeShader = std::make_unique<Shader>("framebuffer.vert", "bloomComposite.frag");

    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    glGenBuffers(1, &m_quadVBO);
    glGenVertexArrays(1, &m_quadVAO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    glGenFramebuffers(1, &m_brightFBO);
    glGenTextures(1, &m_brightTexture);
    glGenFramebuffers(2, m_blurFBO);
    glGenTextures(2, m_blurTexture);

    initFBOs();
}

void Bloom::initFBOs() {
    // Bright extract FBO (half-res)
    glBindFramebuffer(GL_FRAMEBUFFER, m_brightFBO);
    glBindTexture(GL_TEXTURE_2D, m_brightTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_halfSize.x, m_halfSize.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brightTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("BLOOM::FRAMEBUFFER:: Bright extract framebuffer is not complete!");

    // Blur ping-pong FBOs (half-res)
    for (uint32_t i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_halfSize.x, m_halfSize.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("BLOOM::FRAMEBUFFER:: Blur ping-pong framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::ExtractAndBlur(GLuint sceneTexture) {
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(m_quadVAO);

    glViewport(0, 0, m_halfSize.x, m_halfSize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, m_brightFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    m_extractShader->Use();
    m_extractShader->SetInt("sceneTexture", 0);
    m_extractShader->SetFloat("threshold", m_threshold);
    m_extractShader->SetFloat("softKnee", m_softKnee);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_blurShader->Use();
    m_blurShader->SetInt("image", 0);

    bool horizontal = true;
    for (uint32_t i = 0; i < m_passes; i++) {
        horizontal = ((i ^ 1) == (i + 1));
        m_blurShader->SetBool("horizontal", horizontal);
        m_blurShader->SetFloat("blurScale", m_blurScale);

        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[horizontal ? 1 : 0]);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        if (i == 0) {
            glBindTexture(GL_TEXTURE_2D, m_brightTexture);
        } else {
            glBindTexture(GL_TEXTURE_2D, m_blurTexture[horizontal ? 0 : 1]);
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
}

void Bloom::Composite(GLuint sceneTexture, GLuint targetFBO) {
    glViewport(0, 0, m_screenSize.x, m_screenSize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_compositeShader->Use();
    m_compositeShader->SetInt("sceneTexture", 0);
    m_compositeShader->SetInt("bloomTexture", 1);
    m_compositeShader->SetFloat("intensity", m_intensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    glActiveTexture(GL_TEXTURE1);
    bool lastHorizontal = ((m_passes - 1) ^ 1) == m_passes;
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[lastHorizontal ? 1 : 0]);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Bloom::ScreenResize(glm::ivec2 screenSize) {
    m_screenSize = screenSize;
    m_halfSize = screenSize / 2;
    initFBOs();
}

Bloom::~Bloom() {
    glDeleteFramebuffers(1, &m_brightFBO);
    glDeleteTextures(1, &m_brightTexture);
    glDeleteFramebuffers(2, m_blurFBO);
    glDeleteTextures(2, m_blurTexture);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

} // namespace Filter
