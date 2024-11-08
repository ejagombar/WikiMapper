#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "../../lib/camera.hpp"
#include "../../lib/skybox.hpp"
#include "./filter.hpp"
#include <GL/gl.h> // This header isn't required as glad already provides it, however if it is not here, then the the language server automatically adds it when autocomplete is used on a OpenGL function
#include <GLFW/glfw3.h>
#include <math.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../lib/texture.hpp"

enum State { play, pause };

class gui {
  public:
    gui() { init(); };
    ~gui() {};

  private:
    int init();
    void engine_loop();

    void processEngineInput(GLFWwindow *window);
    void processInput();

    static void framebuffer_size_callback_static(GLFWwindow *window, int width, int height);
    static void mouse_callback_static(GLFWwindow *window, double xpos, double ypos);
    static void scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset);
    static void key_callback_static(GLFWwindow *window, int key, int scancode, int action, int mods);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    const unsigned int m_SCR_WIDTH = 1920;
    const unsigned int m_SCR_HEIGHT = 1080;

    float m_lastX;
    float m_lastY;
    bool m_firstMouse = true;

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    State m_state = play;

    Camera m_camera;
    GLFWwindow *m_window;

    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_grassShader;
    std::unique_ptr<Shader> m_lightShader;
    std::unique_ptr<Shader> m_skyboxShader;
    std::unique_ptr<Shader> m_screenShaderBlur;
    std::unique_ptr<Shader> m_screenShaderMix;

    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<Filter::Blur> m_blur;

    std::vector<glm::vec3> m_vegetation;
    std::vector<glm::vec3> m_cubePositions;

    unsigned int m_grassTexture;
    static const uint8_t count = 3;
    unsigned int m_VAOs[count], m_VBOs[count];
};
