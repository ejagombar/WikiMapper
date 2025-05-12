#include "./engine.hpp"
#include "../../lib/rgb_hsv.hpp"
#include "../logger.hpp"
#include "./shader.hpp"
#include "camera.hpp"
#include "gui.hpp"
#include "texture.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <future>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

// This constructor sets up the graphical window, initialises buffers and textures, and creates the GUI. The debugData
// and associated mutex is quite messy and a temporary solution. This shall be changed eventually.
Engine::Engine(GS::GraphTripleBuf &graphBuf, ControlData &controlData)
    : m_controlData(controlData), m_graphBuf(graphBuf) {

    globalLogger->info("Initialising Engine");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(m_scrWidth, m_scrHeight, "WikiMapper", NULL, NULL);
    if (m_window == NULL) {
        globalLogger->error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        globalLogger->error("Failed to initialize GLAD");
    }

    glViewport(0, 0, m_scrWidth, m_scrHeight);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback_static);
    glfwSetKeyCallback(m_window, key_callback_static);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback_static);
    glfwSetCursorPosCallback(m_window, mouse_callback_static);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_camera.SetPosition(glm::vec3(25.0f, 0.0f, 0.0f), glm::pi<float>(), 0.0f);
    m_camera.SetAspectRatio(static_cast<float>(m_scrWidth) / static_cast<float>(m_scrHeight));

    m_gui = std::make_unique<GUI>(m_window, m_font, m_controlData);

    setupShaders();

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    initNodeBuffers();
    initEdgeBuffers();

    GS::Graph *graph = m_graphBuf.GetCurrent();

    updateNodes(*graph);
    updateEdges(*graph);

    m_text->SetupTextureAtlases(graph->nodes);
    m_text->UpdateLabelPositions(graph->nodes);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);
}

// Initialises all the shaders. Each shader is assigned its own shader class which would be inefficient if there were
// many types of shaders, however as there are only a couple shaders, this does not have a big impact.
void Engine::setupShaders() {
    globalLogger->info("Setting up shaders");
    m_shader.skybox = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_shader.screenBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_shader.sphere = std::make_unique<Shader>("sphere.vert", "sphere.frag", "sphere.geom");
    m_shader.cylinder = std::make_unique<Shader>("cylinder.vert", "cylinder.frag", "cylinder.geom");

    m_blur = std::make_unique<Filter::Blur>(*m_shader.screenBlur, glm::ivec2(m_scrWidth, m_scrHeight),
                                            glm::ivec2(1000, 800), 100, false, .5f, 14, 0.92f);

    m_text = std::make_unique<LabelEngine>(m_font, "label.vert", "label.frag", "label.geom");

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

void Engine::initNodeBuffers() {
    glBindVertexArray(m_VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);

    GLint colorRadiusAttr = m_shader.sphere->GetAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(colorRadiusAttr);
    glVertexAttribPointer(colorRadiusAttr, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NodeData),
                          reinterpret_cast<void *>(offsetof(NodeData, r)));

    const size_t nodePosOffset = offsetof(NodeData, position);

    GLint posAttr = m_shader.sphere->GetAttribLocation("aPos");
    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(NodeData), reinterpret_cast<void *>(nodePosOffset));

    glBindVertexArray(0);
}

void Engine::initEdgeBuffers() {
    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);

    GLint colorRadiusAttrib = m_shader.cylinder->GetAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(colorRadiusAttrib);
    glVertexAttribPointer(colorRadiusAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(EdgeData),
                          reinterpret_cast<void *>(offsetof(EdgeData, r)));

    const size_t edgePosOffset = offsetof(EdgeData, position);

    GLint posAttr = m_shader.cylinder->GetAttribLocation("aPos");
    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(EdgeData), reinterpret_cast<void *>(edgePosOffset));

    glBindVertexArray(0);
}

// Initialise buffer with node data on the GPU
void Engine::updateNodes(GS::Graph &graph) {
    globalLogger->info("Start updating nodes");
    uint32_t nodeCount = uint32_t(graph.nodes.size());
    m_nodeData.resize(nodeCount);

    for (uint32_t i = 0; i < nodeCount; ++i) {
        const auto &src = graph.nodes[i];
        m_nodeData[i] = {src.rgb[0], src.rgb[1], src.rgb[2], GLubyte(src.size), {src.pos.x, src.pos.y, src.pos.z}};
    }

    // Orphan and then refill GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, m_nodeData.size() * sizeof(NodeData), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nodeData.size() * sizeof(NodeData), m_nodeData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    globalLogger->info("Finish updating nodes");
}

// Initialise buffer with edge data on the GPU.
void Engine::updateEdges(GS::Graph &graph) {
    globalLogger->info("Start updating edges");
    uint32_t edgeCount = uint32_t(graph.edges.size());
    m_edgeData.resize(edgeCount * 2);

    for (uint32_t i = 0; i < edgeCount; ++i) {
        const auto &s = graph.EdgeStart(i);
        const auto &e = graph.EdgeEnd(i);
        m_edgeData[2 * i] = {s.rgb[0], s.rgb[1], s.rgb[2], GLubyte(s.edgeSize), {s.pos.x, s.pos.y, s.pos.z}};
        m_edgeData[2 * i + 1] = {e.rgb[0], e.rgb[1], e.rgb[2], GLubyte(e.edgeSize), {e.pos.x, e.pos.y, e.pos.z}};
    }

    // Orphan and then refill GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, m_edgeData.size() * sizeof(EdgeData), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_edgeData.size() * sizeof(EdgeData), m_edgeData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    globalLogger->info("Finish updating edges");
}

// Update node and edge buffers with new position data. Future optimisation to keep in mind: pack the position data
// together so it can be copied all at once as opposed to looping through each value.
void Engine::updateParticles(GS::Graph &graph) {
    uint32_t minNodeCount = std::min(m_nodeData.size(), graph.nodes.size());
    for (uint32_t i = 0; i < minNodeCount; i++) {
        const auto &src = graph.nodes.at(i).pos;
        auto &dst = m_nodeData.at(i).position;

        dst[0] = src.x;
        dst[1] = src.y;
        dst[2] = src.z;
    }

    m_text->UpdateLabelPositions(graph.nodes);

    uint32_t minEdgeCount = std::min(m_edgeData.size() / 2, graph.edges.size());
    for (uint32_t i = 0; i < minEdgeCount; i++) {
        const uint32_t lineIdx = i * 2;

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

    globalLogger->info("Execution Time: ", glfwGetTime() - m_startTime);

    glfwTerminate();
}

uint32_t Engine::Run() {
    double lastTime = glfwGetTime();
    m_startTime = lastTime;

    // glfwSwapInterval(0);

    bool textureGenTriggered = false;
    std::future<void> fut;

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();

        m_frameCount++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            printf("%f fps\n", double(m_frameCount));
            m_frameCount = 0;
            lastTime += 1.0;

            if (m_controlData.engine.initGraphData.load(std::memory_order_relaxed)) {
                m_controlData.engine.initGraphData.store(false, std::memory_order_relaxed);
                GS::Graph *graph = m_graphBuf.GetCurrent();

                updateNodes(*graph);
                updateEdges(*graph);

                m_text->SetupTextureAtlases(graph->nodes);
            }
        }

        std::cout << "Loop" << std::endl;

        loop();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    return 0;
}

// The engine main loop. This is called to render each frame. It also processes any user inputs, and updates node
// positions if required (if a new graph position frame has been generated).
void Engine::loop() {
    m_gui->BeginFrame();

    m_camera.SetFov(m_controlData.engine.cameraFov);
    m_camera.SetMouseSensitivity(m_controlData.engine.mouseSensitivity);

    processEngineInput(m_window);
    const float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (m_state == stop)
        deltaTime = 0;

    uint32_t currentVersion = m_graphBuf.Version();
    if (currentVersion != m_lastVersion) {
        m_graph = m_graphBuf.GetCurrent();
        updateParticles(*m_graph);
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

    m_text->RenderLabels(currentFrame);

    m_shader.cylinder->Use();
    m_shader.cylinder->SetMat3("normalMat", normal);
    m_shader.cylinder->SetFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_LINES, 0, m_graph->edges.size() * 2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_gui->RenderSearchBar();
    m_gui->RenderBottomLeftBox();

    if (m_gui->GUIValues().debugMode) {
        m_gui->RenderDebugMenu();
    }

    m_blur->Display();

    if (m_state == stop) {
        m_gui->RenderMenu();
    }

    m_gui->EndFrame();
}

// -------------------------------- Static Callbacks --------------------------------
// Static functions must be used for callbacks so these static functions wrap the functions used in the engine
// object, so that the functions that are called are able to access engine member variables.

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

void Engine::mouse_button_callback_static(GLFWwindow *window, int button, int action, int mods) {
    Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->mouse_button_callback(window, button, action, mods);
    }
}

// -------------------------------- Callbacks --------------------------------

// A check if made to the GUI to see if it is active before processing the key strokes. If it is active, the
// function will return early. The "q" key is an exception to this as this opens and closes the menu so this should
// work when the menu is active or not. This leads to a very small bug when typing into the search box, if you press
// q and the menu is open, it will close the menu, but this will not happen for any other letter.
void Engine::key_callback([[maybe_unused]] GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action,
                          [[maybe_unused]] int mods) {

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if (m_state == play && !m_gui->Active()) {
            m_state = stop;
            m_blur->SetEnabled(true);
        } else {
            m_state = play;
            m_blur->SetEnabled(false);
        }
    }

    if (m_gui->Active()) {
        return;
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, 1);
    }
}

// Called when the window size changes
void Engine::framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    m_blur->ScreenResize(glm::ivec2(width, height));
    m_text2d->UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));

    m_scrWidth = width;
    m_scrHeight = height;
}

// Called when there is mouse movement
void Engine::mouse_callback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos) {
    if (m_state == stop || !m_mouseActive) {
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

void Engine::mouse_button_callback([[maybe_unused]] GLFWwindow *window, int button, int action,
                                   [[maybe_unused]] int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_firstMouse = true;
        m_mouseActive = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_mouseActive = false;
    }
}

void Engine::processEngineInput(GLFWwindow *window) {
    if (m_gui->Active()) {
        return;
    }

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
