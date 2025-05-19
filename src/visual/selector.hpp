#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include "./shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class SelectorSystem {
  public:
    SelectorSystem(int width, int height);
    ~SelectorSystem();

    void Resize(int width, int height);
    void Begin();
    void End();
    int ReadNodeID(int x, int y);

    std::unique_ptr<Shader> pickingShader;

  private:
    GLuint m_fbo;
    GLuint m_colorTexture;
    GLuint m_depthRenderBuffer;
    int m_width;
    int m_height;

    void InitFramebuffer(int width, int height);
};

#endif // SELECTOR_HPP
