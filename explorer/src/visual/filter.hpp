#include "../../lib/shader.hpp"
#include <GL/gl.h>
#include <sys/types.h>

namespace Filter {
class Blur {
  public:
    Blur(Shader &blurShader, GLuint screenWidth, GLuint screenHeight, bool enabled = false);

    void Preprocess();
    void Display();

    void SetEnabled(const bool enabled) { m_enabled = enabled; }
    bool GetEnabled() const { return m_enabled; }

    void Resize(const GLuint screenWidth, const GLuint screenHeight);

  private:
    void initSizeDependantBuffers();

    Shader &m_blurShader;

    GLuint m_sceneFBO, m_sceneTexture;
    GLuint m_blurFBO[2], m_blurTexture[2]; // Horizontal and Vertical blur
    GLuint m_quadVAO;

    GLint m_screenWidth;
    GLint m_screenHeight;

    bool m_enabled = false;

    GLint m_boarder = 100;      // Boarder size around blur area
    GLint m_radius = 50;        // Blur rectangle corner radius
    GLfloat m_blurScale = 5.0f; // Guassian blur step size
    GLint m_numBlurPasses = 4;  // Number of Guassian blur passes
    GLfloat m_brightnessModifier = 0.9;
};
} // namespace Filter
