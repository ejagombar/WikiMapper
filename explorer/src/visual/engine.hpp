#ifndef ENGINE_H
#define ENGINE_H

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
    glm::vec3 pos;
    unsigned char rgb[3];
    float size;
    std::string text;
};

struct Edge {
    glm::vec3 start, end;
    unsigned char startRGB[3];
    unsigned char endRGB[3];
    float size;
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

struct CameraMatrices {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 position;
};

enum State { play, stop };

class Engine {
  public:
    Engine(const int &maxNodes, std::vector<Node> &nodes, std::vector<Edge> &lines);
    ~Engine();
    int Run();

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

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

    void setupNodes();
    void setupEdges();

    unsigned int m_scrWidth = 1920;
    unsigned int m_scrHeight = 1080;

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

    const GLuint m_CAMERA_MATRICES_UBO_BINDING_POINT = 0;
    const GLuint m_ENVIRONMENT_LIGHTING_UBO_BINDING_POINT = 1;

    std::unique_ptr<UBOManager<CameraMatrices>> m_cameraMatricesUBO;
    std::unique_ptr<UBOManager<EnvironmentLighting>> m_environmentUBO;

    std::vector<Node> &m_nodes;
    std::vector<Edge> &m_lines;

    GLuint m_maxNodes;
    GLuint m_node_buffer;

    GLuint m_lineCount;
    GLuint m_nodeCount;

    static const uint8_t count = 3;
    unsigned int m_VAOs[count], m_VBOs[count];
};

#endif
