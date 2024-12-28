#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "./camera.hpp"
#include "./filter.hpp"
#include "./skybox.hpp"
#include "./text.hpp"
#include "./uniformBufferObject.hpp"
#include <GL/gl.h> // This header isn't required as glad already provides it, however if it is not here, then the the language server automatically adds it when autocomplete is used on a OpenGL function
#include <GLFW/glfw3.h>
#include <math.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./texture.hpp"
#include <fstream>

// #define RecordCameraMovement true
// #define ReplayCameraMovement true

template <typename T> std::vector<T> ReadFileData(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    size_t numEntries = fileSize / sizeof(T);

    std::vector<T> data(numEntries);
    file.read(reinterpret_cast<char *>(data.data()), fileSize);

    if (!file) {
        throw std::runtime_error("Error reading data from file.");
    }

    return data;
}

struct Node {
    glm::vec3 pos, speed;
    unsigned char r, g, b, a; // Color
    float size, angle, weight;
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f
    std::string text;

    bool operator<(const Node &that) const {
        // Sort in reverse order : far nodes drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

struct PointLight {
    alignas(16) glm::vec3 Position;
    alignas(16) glm::vec3 Color;
    alignas(4) float Constant;
    alignas(4) float Linear;
    alignas(4) float Quadratic;
};

// Struct matching the GlobalUniforms block in the GLSL shader
struct GlobalUniforms {
    alignas(16) glm::vec3 GlobalLightColor;
    alignas(16) glm::vec3 GlobalLightDir;
    alignas(4) int NumPointLights;
    alignas(16) PointLight PointLights[4];
};

struct CameraMatrices {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::vec4 Position;
};

enum State { play, pause };

class Engine {
  public:
    Engine(const int &MaxNodes, std::vector<Node> &nodes, std::vector<glm::vec3> &lines);
    ~Engine();
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

    std::fstream m_positionFile;
    int m_recordCount = 0;
    std::fstream m_benchmarkTimestamps;
    std::vector<CameraPositionData> m_camPosData;
    std::vector<double> m_benchmarkTimestampsData;
    int m_frameCount = 0;
    int m_startFrameTime;
    bool m_benchmarkRecord = false;

    float m_lastX;
    float m_lastY;
    bool m_firstMouse = true;

    double m_startTime;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
    double m_lastCameraRecord = 0.0;

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

    const GLuint m_GLOBAL_UNIFORM_BINDING_POINT = 0;
    const GLuint m_LIGHTING_UBO = 1;
    std::unique_ptr<UBOManager<CameraMatrices>> m_globalUBO;
    std::unique_ptr<UBOManager<GlobalUniforms>> m_EnvironmentUBO;

    std::vector<Node> m_nodes;

    GLuint m_MaxNodes;
    GLuint m_node_buffer;

    GLuint m_lineCount;
    GLuint m_nodeCount;

    static const uint8_t count = 3;
    unsigned int m_VAOs[count], m_VBOs[count];
};
