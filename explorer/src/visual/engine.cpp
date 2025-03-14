#include "./engine.hpp"

#include "../../lib/rgb_hsv.hpp"
#include "./shader.hpp"
#include "camera.hpp"
#include "gui.hpp"
#include "texture.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <random>
#include <vector>

Engine::Engine(GS::GraphTripleBuf &graphBuf, debugData &simDebugData, std::mutex &simDebugDataMutex)
    : m_simDebugData(simDebugData), m_simDebugDataMutex(simDebugDataMutex), m_graphBuf(graphBuf) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_scrWidth, m_scrHeight, "WikiMapper", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, m_scrWidth, m_scrHeight);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback_static);
    glfwSetKeyCallback(m_window, key_callback_static);
    // glfwSetScrollCallback(m_window, scroll_callback_static);
    glfwSetCursorPosCallback(m_window, mouse_callback_static);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_camera.SetPosition(glm::vec3(25.0f, 0.0f, 0.0f), glm::pi<float>(), 0.0f);
    m_camera.SetAspectRatio(static_cast<float>(m_scrWidth) / static_cast<float>(m_scrHeight));

    m_gui = std::make_unique<GUI>(m_window, m_font);

    GS::Graph *graph = m_graphBuf.GetCurrent();

    setupShaders(*graph);

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    setupEdges(*graph);
    setupNodes(*graph);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);
}

void Engine::setupShaders(GS::Graph &graph) {
    m_shader.skybox = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_shader.screenBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_shader.sphere = std::make_unique<Shader>("sphere.vert", "sphere.frag", "sphere.geom");
    m_shader.cylinder = std::make_unique<Shader>("cylinder.vert", "cylinder.frag", "cylinder.geom");

    m_blur = std::make_unique<Filter::Blur>(*m_shader.screenBlur, glm::ivec2(m_scrWidth, m_scrHeight),
                                            glm::ivec2(1000, 800), 100, false, .5f, 14, 0.92f);

    std::vector<Label> labels;
    for (GS::Node node : graph.nodes) {
        labels.emplace_back(Label{node.pos, node.title});
    }

    m_text = std::make_unique<LabelEngine>(m_font, "label.vert", "label.frag", "label.geom", labels);

    m_shader.cameraMatricesUBO =
        std::make_unique<UBOManager<CameraMatrices>>(m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.environmentUBO =
        std::make_unique<UBOManager<EnvironmentLighting>>(m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);

    m_shader.sphere->LinkUBO("GlobalUniforms", m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.cylinder->LinkUBO("GlobalUniforms", m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.sphere->LinkUBO("EnvironmentUniforms", m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);
    m_shader.cylinder->LinkUBO("EnvironmentUniforms", m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);

    GLuint cubemapTexture = LoadCubemap(std::vector<std::string>{"stars.jpg"});

    m_skybox = std::make_unique<Skybox>(*m_shader.skybox, cubemapTexture);
}

void Engine::setupNodes(GS::Graph &graph) {
    GLuint nodeCount = graph.nodes.size();
    m_nodeData.resize(nodeCount);

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (uint i = 0; i < nodeCount; i++) {
        m_nodeData[i].r = graph.nodes[i].rgb[0];
        m_nodeData[i].g = graph.nodes[i].rgb[1];
        m_nodeData[i].b = graph.nodes[i].rgb[2];
        m_nodeData[i].radius = static_cast<GLubyte>(graph.nodes[i].size);
        m_nodeData[i].position[0] = graph.nodes[i].pos.x;
        m_nodeData[i].position[1] = graph.nodes[i].pos.y;
        m_nodeData[i].position[2] = graph.nodes[i].pos.z;
    }

    glBindVertexArray(m_VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, m_nodeData.size() * sizeof(NodeData), m_nodeData.data(), GL_DYNAMIC_DRAW);

    const GLint aColorAttrib = m_shader.sphere->GetAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(aColorAttrib);
    glVertexAttribPointer(aColorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NodeData), (void *)0);

    const GLint aPosAttrib = m_shader.sphere->GetAttribLocation("aPos");
    glEnableVertexAttribArray(aPosAttrib);
    glVertexAttribPointer(aPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(NodeData), (void *)(sizeof(float)));

    glBindVertexArray(0);
}

void Engine::setupEdges(GS::Graph &graph) {
    GLuint edgeCount = graph.edges.size();

    m_edgeData.resize(edgeCount * 2);
    for (uint i = 0; i < edgeCount; i++) {
        const int lineIdx = i * 2;

        m_edgeData[lineIdx].r = graph.EdgeStart(i).rgb[0];
        m_edgeData[lineIdx].g = graph.EdgeStart(i).rgb[1];
        m_edgeData[lineIdx].b = graph.EdgeStart(i).rgb[2];
        m_edgeData[lineIdx].radius = graph.EdgeStart(i).edgeSize;
        m_edgeData[lineIdx].position[0] = graph.EdgeStart(i).pos.x;
        m_edgeData[lineIdx].position[1] = graph.EdgeStart(i).pos.y;
        m_edgeData[lineIdx].position[2] = graph.EdgeStart(i).pos.z;

        m_edgeData[lineIdx + 1].r = graph.EdgeEnd(i).rgb[0];
        m_edgeData[lineIdx + 1].g = graph.EdgeEnd(i).rgb[1];
        m_edgeData[lineIdx + 1].b = graph.EdgeEnd(i).rgb[2];
        m_edgeData[lineIdx + 1].radius = graph.EdgeEnd(i).edgeSize;
        m_edgeData[lineIdx + 1].position[0] = graph.EdgeEnd(i).pos.x;
        m_edgeData[lineIdx + 1].position[1] = graph.EdgeEnd(i).pos.y;
        m_edgeData[lineIdx + 1].position[2] = graph.EdgeEnd(i).pos.z;
    }

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, m_edgeData.size() * sizeof(EdgeData), m_edgeData.data(), GL_DYNAMIC_DRAW);

    const GLint radiusAttrib = m_shader.cylinder->GetAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(radiusAttrib);
    glVertexAttribPointer(radiusAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(EdgeData), (void *)0);

    const GLint startAttrib = m_shader.cylinder->GetAttribLocation("aPos");
    glEnableVertexAttribArray(startAttrib);
    glVertexAttribPointer(startAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(EdgeData), (void *)(sizeof(float)));

    glBindVertexArray(0);
}

void Engine::UpdateParticles(GS::Graph &graph) {
    for (unsigned int i = 0; i < graph.nodes.size(); i++) {
        m_nodeData[i].position[0] = graph.nodes[i].pos.x;
        m_nodeData[i].position[1] = graph.nodes[i].pos.y;
        m_nodeData[i].position[2] = graph.nodes[i].pos.z;
    }

    for (unsigned int i = 0; i < graph.edges.size(); i++) {
        const int lineIdx = i * 2;

        m_edgeData[lineIdx].position[0] = graph.EdgeStart(i).pos.x;
        m_edgeData[lineIdx].position[1] = graph.EdgeStart(i).pos.y;
        m_edgeData[lineIdx].position[2] = graph.EdgeStart(i).pos.z;

        m_edgeData[lineIdx + 1].position[0] = graph.EdgeEnd(i).pos.x;
        m_edgeData[lineIdx + 1].position[1] = graph.EdgeEnd(i).pos.y;
        m_edgeData[lineIdx + 1].position[2] = graph.EdgeEnd(i).pos.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NodeData) * m_nodeData.size(), m_nodeData.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_edgeData.size() * sizeof(EdgeData), m_edgeData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Engine::~Engine() {
    glDeleteVertexArrays(count, m_VAOs);
    glDeleteBuffers(count, m_VBOs);

    std::cout << "Execution Time: " << glfwGetTime() - m_startTime << std::endl;

    glfwTerminate();
}

int Engine::Run() {
    double lastTime = glfwGetTime();
    m_startTime = lastTime;

    // glfwSwapInterval(0);

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();

        m_frameCount++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            printf("%f fps\n", double(m_frameCount));
            m_frameCount = 0;
            lastTime += 1.0;
        }

        loop();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    return 0;
}

void Engine::loop() {
    m_gui->BeginFrame();

    m_camera.SetFov(m_gui->GUIValues().cameraFov);
    m_camera.SetMouseSensitivity(m_gui->GUIValues().mouseSensitivity);

    if (m_simDebugDataMutex.try_lock()) {
        GUISettings x = m_gui->GUIValues();
        m_simDebugData.accelSizeMultiplier = x.accelSizeMultiplier;
        m_simDebugData.gravityMultiplier = x.gravityMultiplier;
        m_simDebugData.qqMultiplier = x.qqMultiplier;
        m_simDebugData.targetDistance = x.targetDistance;
        m_simDebugDataMutex.unlock();
    }

    processEngineInput(m_window);
    const float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (m_state == stop)
        deltaTime = 0;

    uint currentVersion = m_graphBuf.Version();
    if (currentVersion != m_lastVersion) {
        m_graph = m_graphBuf.GetCurrent();
        UpdateParticles(*m_graph);
        m_lastVersion = currentVersion;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_blur->Preprocess();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_camera.ProcessPosition(deltaTime);

    glm::vec3 cameraPosition = m_camera.GetCameraPosition();
    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 view = m_camera.GetViewMatrix();
    glm::mat3 normal = m_camera.CalcNormalMatrix();

    const CameraMatrices cameraMatrices{projection, view, glm::vec4(cameraPosition, 1.0)};

    const glm::mat4 cameraDirection = cameraMatrices.projection * glm::mat4(glm::mat3(view));

    EnvironmentLighting uniforms = {};
    uniforms.globalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    uniforms.globalLightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    uniforms.pointLightCount = 2;

    uniforms.pointLight[0] = {cameraPosition, glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 0.09f, 0.032f};
    uniforms.pointLight[1] = {glm::vec3(-2.0f, 1.0f, -1.0f), glm::vec3(0.5f, 0.5f, 1.0f), 1.0f, 0.07f, 0.017f};

    m_shader.environmentUBO->Update(uniforms);

    m_skybox->Display(cameraDirection);

    m_shader.cameraMatricesUBO->Update(cameraMatrices);

    m_shader.sphere->Use();
    m_shader.sphere->SetFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_graph->nodes.size());

    m_text->RenderLabels(view, currentFrame);

    m_shader.cylinder->Use();
    m_shader.cylinder->SetMat3("normalMat", normal);
    m_shader.cylinder->SetFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_LINES, 0, m_graph->edges.size() * 2);

    m_blur->Display();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_state == stop) {
        m_gui->RenderMenu();
    }
    if (m_gui->GUIValues().debugMode) {
        m_gui->RenderDebugMenu();
    }

    m_gui->EndFrame();
}

void Engine::key_callback_static(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->key_callback(window, key, scancode, action, mods);
    }
}

void Engine::framebuffer_size_callback_static(GLFWwindow *window, int width, int height) {
    Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->framebuffer_size_callback(window, width, height);
    }
}

void Engine::mouse_callback_static(GLFWwindow *window, double xpos, double ypos) {
    Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->mouse_callback(window, xpos, ypos);
    }
}

// void Engine::scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset) {
//     Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
//     if (instance) {
//         instance->scroll_callback(window, xoffset, yoffset);
//     }
// }

// void Engine::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
// }

void Engine::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    m_blur->ScreenResize(glm::ivec2(width, height));
    // m_text2d->UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));

    m_scrWidth = width;
    m_scrHeight = height;
}

void Engine::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (m_state == stop) {
        return;
    }

    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void Engine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if (m_state == play) {
            m_state = stop;
            m_blur->SetEnabled(true);

            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            m_state = play;
            m_blur->SetEnabled(false);

            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_firstMouse = true;
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, 1);
    }
}

void Engine::processEngineInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::UP);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::DOWN);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_camera.ProcessKeyboard(CameraMovement::SNEAK);
}
