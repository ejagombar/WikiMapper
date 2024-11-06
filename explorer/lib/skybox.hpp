#include "../lib/camera.h"
#include "../lib/shader.h"
#include <sys/types.h>

class Skybox {
  public:
    Skybox(Shader &skyboxShader, uint cubemapTexture);

    void Preprocess();
    void Display(Camera camera);

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool GetEnabled() { return m_enabled; }

  private:
    Shader &m_skyboxShader;
    bool m_enabled = false;
    uint m_skyboxVAO;
    uint m_cubemapTexture;
};
