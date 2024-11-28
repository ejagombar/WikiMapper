#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "./camera.hpp"
#include "./filter.hpp"
#include "./skybox.hpp"
#include "./text.hpp"
#include <GL/gl.h> // This header isn't required as glad already provides it, however if it is not here, then the the language server automatically adds it when autocomplete is used on a OpenGL function
#include <GLFW/glfw3.h>
#include <math.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./texture.hpp"

struct Node {
    glm::vec3 pos, speed;
    unsigned char r, g, b, a; // Color
    float size, angle, weight;
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Node &that) const {
        // Sort in reverse order : far nodes drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

enum State { play, pause };

class GUI {
  public:
    GUI(const int &MaxNodes, std::vector<Node> &nodes);
    ~GUI();
    int run();

  private:
    void loop();

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

    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

    unsigned int m_ScrWidth = 1920;
    unsigned int m_ScrHeight = 1080;

    float m_lastX;
    float m_lastY;
    bool m_firstMouse = true;

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    State m_state = play;

    Camera m_camera;
    GLFWwindow *m_window;

    std::unique_ptr<Shader> m_skyboxShader;
    std::unique_ptr<Shader> m_screenShaderBlur;
    std::unique_ptr<Shader> m_sphereShader;
    std::unique_ptr<Shader> m_lineShader;

    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<Filter::Blur> m_blur;

    std::unique_ptr<Text> m_text;
    std::unique_ptr<Text2d> m_text2d;

    GLuint m_MaxNodes;
    GLuint m_node_buffer;

    static const uint8_t count = 3;
    unsigned int m_VAOs[count], m_VBOs[count];
};
