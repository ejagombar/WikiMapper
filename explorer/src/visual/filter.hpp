#include "../../lib/shader.hpp"
#include <sys/types.h>

namespace Filter {
class Blur {
  public:
    Blur(Shader &blurShader, Shader &mixShader, uint screenWidth, uint screenHeight, bool enabled = false);

    void Preprocess();
    void Display();

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool GetEnabled() { return m_enabled; }

  private:
    Shader &m_blurShader;
    Shader &m_mixShader;

    unsigned int m_sceneFBO;
    unsigned int m_blurFBO[2], m_blurTexture[2];
    unsigned int m_quadVAO;
    unsigned int m_sceneTexture;

    int m_screenWidth;
    int m_screenHeight;

    bool m_enabled = false;

    int m_boarder = 100;      // Boarder size around blur area
    int m_radius = 50;        // Blur rectangle corner radius
    float m_blurScale = 5.0f; // Guassian blur step size
    float m_blurMix = 0.98f;  // Ratio of mix between blur and normal scene
    int m_numBlurPasses = 6;  // Number of Guassian blur passes
    float m_brightnessModifier = 0.9;
};
} // namespace Filter
