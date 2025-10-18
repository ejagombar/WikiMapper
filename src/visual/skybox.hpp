#ifndef SKYBOX_H
#define SKYBOX_H

#include "./shader.hpp"
#include <sys/types.h>

class Skybox {
  public:
    Skybox(Shader &skyboxShader, const GLuint cubemapTexture);
    ~Skybox();

    void Display(const glm::mat4 camera_direction);

    void SetCubemapTexture(const GLuint newCubemapTexture);

    void SetEnabled(const bool enabled) { m_enabled = enabled; }
    bool GetEnabled() const { return m_enabled; }

  private:
    Shader &m_skyboxShader;
    bool m_enabled = false;
    GLuint m_skyboxVAO;
    GLuint m_skyboxVBO;
    GLuint m_cubemapTexture;
};
#endif
