#include "../../lib/shader.hpp"
#include <GL/gl.h>
#include <glm/detail/qualifier.hpp>
#include <sys/types.h>

namespace Filter {
class Blur {
  public:
    Blur(Shader &shader, glm::ivec2 screenSize, glm::ivec2 size, GLuint radius, bool enabled = false, GLfloat scale = 5,
         uint passes = 2, GLfloat brightnessModifier = 0.95f);
    ~Blur();

    void Preprocess();
    void Display();

    void SetEnabled(const bool enabled) { m_enabled = enabled; }
    bool GetEnabled() const { return m_enabled; }

    void ScreenResize(const glm::ivec2 screenSize);

    void SetSize(const glm::ivec2 size) { m_size = size; }
    void SetRadius(const GLuint radius) { m_radius = radius; }
    void SetScale(const GLfloat scale) { m_scale = scale; }
    void SetPasses(const uint passes) { m_passes = passes; }
    void SetBrightness(const GLfloat brightnes) { m_brightnessModifier = brightnes; }

  private:
    void initSizeDependantBuffers();

    Shader &m_blurShader;

    GLuint m_blurFBO[2], m_blurTexture[2]; // Horizontal and Vertical blur
    GLuint m_rboDepth[2];

    GLuint m_quadVAO;
    GLuint m_quadVBO;

    glm::vec2 m_screenSize;

    bool m_enabled = false;

    glm::ivec2 m_size;
    GLuint m_radius; // Blur rectangle corner radius
    GLfloat m_scale; // Guassian blur step size
    uint m_passes;   // Number of Guassian blur passes
    GLfloat m_brightnessModifier;
};
} // namespace Filter
