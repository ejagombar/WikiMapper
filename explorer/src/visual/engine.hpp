#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "../graph.hpp"
#include "./camera.hpp"
#include "./filter.hpp"
#include "./label.hpp"
#include "./skybox.hpp"
#include "./text.hpp"
#include "./uniformBufferObject.hpp"
#include <GL/gl.h> // This header isn't required as glad already provides it, however if it is not here, then the the language server automatically adds it when autocomplete is used on a OpenGL function
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <memory>

class Engine {
  public:
    Engine(std::atomic<GS::GraphBuffer *> &graphBuf);
    ~Engine();
    int Run();

    void UpdateParticles();

  private:
    enum State { play, stop };

    struct NodeData {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte radius;
        GLfloat position[3];
    };

    struct EdgeData {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte radius;
        GLfloat position[3];
    };

    struct CameraMatrices {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 position;
    };

    struct PointLight {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(4) float constant;
        alignas(4) float linear;
        alignas(4) float quadratic;
    };

    struct EnvironmentLighting {
        alignas(16) glm::vec3 globalLightColor;
        alignas(16) glm::vec3 globalLightDir;
        alignas(4) int pointLightCount;
        alignas(16) PointLight pointLight[4];
    };

    struct ShaderData {
        const GLuint CAMERA_MATRICES_UBO_BINDING_POINT = 0;
        const GLuint ENVIRONMENT_LIGHTING_UBO_BINDING_POINT = 1;

        std::unique_ptr<UBOManager<CameraMatrices>> cameraMatricesUBO;
        std::unique_ptr<UBOManager<EnvironmentLighting>> environmentUBO;

        std::unique_ptr<Shader> skybox;
        std::unique_ptr<Shader> screenBlur;
        std::unique_ptr<Shader> sphere;
        std::unique_ptr<Shader> cylinder;
    };

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

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

    void setupNodes();
    void setupEdges();
    void setupShaders();

    unsigned int m_scrWidth = 1920;
    unsigned int m_scrHeight = 1080;

    float m_lastX;
    float m_lastY;
    bool m_firstMouse = true;

    int m_frameCount = 0;
    double m_startTime;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    State m_state = play;

    ShaderData m_shader;

    Camera m_camera;
    GLFWwindow *m_window;

    std::unique_ptr<Filter::Blur> m_blur;
    std::unique_ptr<LabelEngine> m_text;
    std::unique_ptr<Text2d> m_text2d;
    std::unique_ptr<Skybox> m_skybox;

    std::atomic<GS::GraphBuffer *> &m_graphBuf;
    GS::Graph &m_graph;
    unsigned int m_lastVersion = 0;

    std::vector<NodeData> m_nodeData;
    std::vector<EdgeData> m_edgeData;

    static const uint8_t count = 2;
    unsigned int m_VAOs[count], m_VBOs[count];
};

#endif
