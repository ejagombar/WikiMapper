#include "../../lib/shader.hpp"
#include <sys/types.h>

namespace Filter {
class Blur {
  public:
    Blur(Shader &blurShader, uint screenWidth, uint screenHeight, bool enabled = false);

    void Preprocess();
    void Display();

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool GetEnabled() { return m_enabled; }

    void Resize(const int screenWidth, const int screenHeight);

  private:
    void initSizeDependantBuffers();

    Shader &m_blurShader;

    unsigned int m_sceneFBO, m_sceneTexture;
    unsigned int m_blurFBO[2], m_blurTexture[2];
    unsigned int m_quadVAO;

    int m_screenWidth;
    int m_screenHeight;

    bool m_enabled = false;

    int m_boarder = 100;      // Boarder size around blur area
    int m_radius = 50;        // Blur rectangle corner radius
    float m_blurScale = 5.0f; // Guassian blur step size
    int m_numBlurPasses = 4;  // Number of Guassian blur passes
    float m_brightnessModifier = 0.9;
};
} // namespace Filter
