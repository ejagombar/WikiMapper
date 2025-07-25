#ifndef ENGINE_H
#define ENGINE_H

#include <cstdint>
#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "../controlData.hpp"
#include "../graph.hpp"
#include "./camera.hpp"
#include "./filter.hpp"
#include "./gui.hpp"
#include "./label.hpp"
#include "./selector.hpp"
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

// This Engine class encapsulates all the data and logic relating to creating a graphical window (with glfw), rendering
// the 3d scene, and handling user input.
class Engine {
  public:
    Engine(GS::GraphTripleBuf &graphBuf, ControlData &controlData);
    ~Engine();
    uint32_t Run();

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

    struct MaterialProperties {
        alignas(4) float specularStrength;
        alignas(4) float shininess;
        alignas(4) float ambient;
    };

    struct ShaderData {
        const GLuint CAMERA_MATRICES_UBO_BINDING_POINT = 0;
        const GLuint ENVIRONMENT_LIGHTING_UBO_BINDING_POINT = 1;
        const GLuint MATERIAL_PROPERTIES_UBO_BINDING_POINT = 2;

        std::unique_ptr<UBOManager<CameraMatrices>> cameraMatricesUBO;
        std::unique_ptr<UBOManager<EnvironmentLighting>> environmentUBO;
        std::unique_ptr<UBOManager<MaterialProperties>> materialUBO;

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
    static void key_callback_static(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse_button_callback_static(GLFWwindow *window, int button, int action, int mods);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    void handleDoubleClick(int action);
    void doubleClickCalled();

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

    void initNodeBuffers();
    void initEdgeBuffers();

    void updateNodes(GS::Graph &graph);
    void updateEdges(GS::Graph &graph);

    void setupShaders();

    void updateParticles(GS::Graph &graph);
    void updateGraphData();

    void updateSelectorBuffer();
    void processMouseSelectorInput(GLFWwindow *window);

    void computeLighting(glm::vec3 cameraPosition);

    void computeHoverTransition(float deltaTime);

    unsigned int m_scrWidth = 1920;
    unsigned int m_scrHeight = 1080;

    float m_lastX;
    float m_lastY;
    bool m_firstMouse = true;

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    // TODO: Change this to ensure it works on other plaforms. Maybe bundle a font?
    const char *m_font = "/usr/share/fonts/open-sans/OpenSans-Regular.ttf";

    State m_state = play;
    bool m_mouseActive = false;

    ShaderData m_shader;

    ControlData &m_controlData;

    Camera m_camera;
    GLFWwindow *m_window;

    std::unique_ptr<Filter::Blur> m_blur;
    std::unique_ptr<LabelEngine> m_text;
    std::unique_ptr<Text2d> m_text2d;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<GUI> m_gui;
    std::unique_ptr<SelectorSystem> m_picking;
    int m_hoveredNodeID = -1;

    GS::GraphTripleBuf &m_graphBuf;
    uint32_t m_lastVersion = 0;
    GS::Graph *m_graph;

    std::vector<NodeData> m_nodeData;
    std::vector<EdgeData> m_edgeData;

    static const uint8_t count = 2;
    unsigned int m_VAOs[count], m_VBOs[count];

    int m_previousHoveredNodeID = -1;
    float m_hoverBrightness = 1.0f;
    const float m_transitionSpeed = 4.0f;
    bool m_inTransition = false;

    int32_t m_selectedNode = -1;
};

#endif
