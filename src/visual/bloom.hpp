#ifndef BLOOM_H
#define BLOOM_H

#include "./shader.hpp"
#include <GL/gl.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <memory>

namespace Filter {
class Bloom {
  public:
    Bloom(glm::ivec2 screenSize, float threshold = 0.5f, float intensity = 0.25f, uint32_t passes = 6,
          float blurScale = 1.5f, float softKnee = 0.5f);
    ~Bloom();

    void ExtractAndBlur(GLuint sceneTexture);
    void Composite(GLuint sceneTexture, GLuint targetFBO);

    void ScreenResize(glm::ivec2 screenSize);

    void SetThreshold(float threshold) { m_threshold = threshold; }
    void SetIntensity(float intensity) { m_intensity = intensity; }
    void SetPasses(uint32_t passes) { m_passes = passes; }
    void SetBlurScale(float scale) { m_blurScale = scale; }

  private:
    void initFBOs();

    std::unique_ptr<Shader> m_extractShader;
    std::unique_ptr<Shader> m_blurShader;
    std::unique_ptr<Shader> m_compositeShader;

    GLuint m_brightFBO, m_brightTexture;
    GLuint m_blurFBO[2], m_blurTexture[2]; // Ping-pong pair

    GLuint m_quadVAO;
    GLuint m_quadVBO;

    glm::ivec2 m_screenSize;
    glm::ivec2 m_halfSize;

    float m_threshold;
    float m_intensity;
    uint32_t m_passes;
    float m_blurScale;
    float m_softKnee;
};
} // namespace Filter

#endif
