
#include "filter.hpp"
#include <GL/gl.h>
#include <stdexcept>

namespace Filter {

Blur::Blur(Shader &blurShader, glm::ivec2 screenSize, glm::ivec2 size, GLuint radius, bool enabled, GLfloat scale,
           uint blurPasses, GLfloat brightnessModifier)
    : m_blurShader(blurShader), m_screenSize(screenSize), m_enabled(enabled), m_size(size), m_radius(radius),
      m_scale(scale), m_passes(blurPasses), m_brightnessModifier(brightnessModifier) {

    // Two triangles that will cover the full screen when rendered in screen space.
    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    assert(m_passes > 0);
    assert(m_scale > 0);
    assert(m_brightnessModifier <= 1.0f);

    glGenBuffers(1, &m_quadVBO);
    glGenVertexArrays(1, &m_quadVAO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
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
    for (GLuint i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);

        // Create the Texture attachment and initialise parameters and memory
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenSize.x, m_screenSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture[i], 0);

        // Create the Renderbuffer and initialise with enough memory
        glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screenSize.x, m_screenSize.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth[i]);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("FILTER::FRAMEBUFFER:: Blur framebuffer is not complete!");
    }

    // Setup original framebuffer and texture to capture the scene.
    glGenFramebuffers(1, &m_originalFBO);
    glGenTextures(1, &m_originalTexture);

    glBindTexture(GL_TEXTURE_2D, m_originalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenSize.x, m_screenSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, m_originalFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_originalTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("FILTER::FRAMEBUFFER:: Original framebuffer is not complete!");

    // Unbind to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Blur::Preprocess() {
    if (!m_enabled) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_originalFBO);
}

void Blur::ScreenResize(const glm::ivec2 screenSize) {
    m_screenSize = screenSize;
    initSizeDependantBuffers();
}

// The blur is applied in alternating directions, reading from one buffer and writing to the other.
// On the final pass, the output is written to the screen (default framebuffer).
void Blur::Display() {
    if (!m_enabled) {
        return;
    }

    m_blurShader.Use();
    m_blurShader.SetInt("width", m_size.x);
    m_blurShader.SetInt("height", m_size.y);
    m_blurShader.SetInt("radius", m_radius);
    m_blurShader.SetFloat("brightnessModifier", m_brightnessModifier);
    m_blurShader.SetInt("screenTexture", 0); // The blur result will be sampled from texture unit 0.

    // Bind the original texture to texture unit 1 for non-blurred areas.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_originalTexture);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              m_rboDepth[0]); // Use the original depth buffer
    m_blurShader.SetInt("originalTexture", 1);
    m_blurShader.SetBool("lastPass", false);

    bool horizontal = true;
    for (uint i = 0; i < m_passes; i++) {
        horizontal = ((i ^ 1) == (i + 1));
        m_blurShader.SetFloat("blurScale", m_scale * float(i));
        m_blurShader.SetBool("horizontal", horizontal);

        if (i == m_passes - 1) {
            m_blurShader.SetBool("lastPass", true);
        }

        glActiveTexture(GL_TEXTURE0);
        if (i == 0) {
            glBindTexture(GL_TEXTURE_2D, m_originalTexture);
        } else {
            glBindTexture(GL_TEXTURE_2D, m_blurTexture[horizontal ? 0 : 1]);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, (i == (m_passes - 1)) ? 0 : m_blurFBO[horizontal ? 1 : 0]);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(m_quadVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
}

Blur::~Blur() {
    glDeleteFramebuffers(1, &m_originalFBO);
    glDeleteTextures(1, &m_originalTexture);
    glDeleteFramebuffers(2, m_blurFBO);
    glDeleteTextures(2, m_blurTexture);
    glDeleteRenderbuffers(2, m_rboDepth);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

} // namespace Filter
