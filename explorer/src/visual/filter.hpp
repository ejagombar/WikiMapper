#include "../../lib/shader.hpp"
#include <GL/gl.h>
#include <sys/types.h>

namespace Filter {
class Blur {
  public:
    Blur(Shader &blurShader, GLuint screenWidth, GLuint screenHeight, bool enabled = false, GLfloat blurScale = 5,
         uint blurPasses = 2, GLfloat brightnessModifier = 0.95f);
    ~Blur();

    void Preprocess();
    void Display();

    void SetEnabled(const bool enabled) { m_enabled = enabled; }
    bool GetEnabled() const { return m_enabled; }

    void Resize(const GLuint screenWidth, const GLuint screenHeight);

  private:
    void initSizeDependantBuffers();

    Shader &m_blurShader;

    GLuint m_blurFBO[2], m_blurTexture[2]; // Horizontal and Vertical blur
    GLuint m_rboDepth[2];

    GLuint m_quadVAO;
    GLuint m_quadVBO;

    GLint m_screenWidth;
    GLint m_screenHeight;

    bool m_enabled = false;

    GLint m_boarder = 100; // Boarder size around blur area
    GLint m_radius = 50;   // Blur rectangle corner radius
    GLfloat m_blurScale;   // Guassian blur step size
    uint m_blurPasses;     // Number of Guassian blur passes
    GLfloat m_brightnessModifier;
};
} // namespace Filter
