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
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <future>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <memory>
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
    m_picking = std::make_unique<SelectorSystem>(m_scrWidth, m_scrHeight);

    setupShaders();

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    initNodeBuffers();
    initEdgeBuffers();

    GS::Graph *graph = m_graphBuf.GetCurrent();

    updateNodes(*graph);
    updateEdges(*graph);

    // m_text->SetupTextureAtlases(graph->nodes);
    auto out = m_text->PrepareLabelAtlases(graph->nodes.titles);
    m_text->UploadLabelAtlasesToGPU(out);

    m_text->UpdateLabelPositions(graph->nodes.positions, graph->nodes.sizes);

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
    m_picking->pickingShader = std::make_unique<Shader>("selector.vert", "selector.frag", "selector.geom");

    m_blur = std::make_unique<Filter::Blur>(*m_shader.screenBlur, glm::ivec2(m_scrWidth, m_scrHeight),
                                            glm::ivec2(1000, 800), 50, false, .5f, 14, 0.92f);

    m_text = std::make_unique<LabelEngine>(m_font, "label.vert", "label.frag", "label.geom");

    m_shader.cameraMatricesUBO =
        std::make_unique<UBOManager<CameraMatrices>>(m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.environmentUBO =
        std::make_unique<UBOManager<EnvironmentLighting>>(m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);
    m_shader.materialUBO =
        std::make_unique<UBOManager<MaterialProperties>>(m_shader.MATERIAL_PROPERTIES_UBO_BINDING_POINT);

    m_shader.sphere->LinkUBO("GlobalUniforms", m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.cylinder->LinkUBO("GlobalUniforms", m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);
    m_shader.sphere->LinkUBO("EnvironmentUniforms", m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);
    m_shader.sphere->LinkUBO("MaterialProperties", m_shader.MATERIAL_PROPERTIES_UBO_BINDING_POINT);
    m_shader.cylinder->LinkUBO("EnvironmentUniforms", m_shader.ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);
    m_picking->pickingShader->LinkUBO("GlobalUniforms", m_shader.CAMERA_MATRICES_UBO_BINDING_POINT);

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
    uint32_t nodeCount = uint32_t(graph.nodes.titles.size());
    m_nodeData.resize(nodeCount);

    for (uint32_t i = 0; i < nodeCount; i++) {
        m_nodeData[i] = {graph.nodes.colors[i].r,
                         graph.nodes.colors[i].g,
                         graph.nodes.colors[i].b,
                         GLubyte(graph.nodes.sizes[i]),
                         {graph.nodes.positions[i].x, graph.nodes.positions[i].y, graph.nodes.positions[i].z}};
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
    uint32_t edgeCount = uint32_t(graph.edges.startIdxs.size());
    m_edgeData.resize(edgeCount * 2);

    for (uint32_t i = 0; i < edgeCount; i++) {
        auto EdgeStart = [&](size_t i) { return; };
        auto EdgeEnd = [&](size_t i) { return; };

        const auto &s = graph.edges.startIdxs.at(i);
        const auto &e = graph.edges.endIdxs.at(i);

        m_edgeData[2 * i] = {graph.nodes.colors[s].r,
                             graph.nodes.colors[s].g,
                             graph.nodes.colors[s].b,
                             GLubyte(graph.nodes.edgeSizes[s]),
                             {graph.nodes.positions[s].x, graph.nodes.positions[s].y, graph.nodes.positions[s].z}};

        m_edgeData[2 * i + 1] = {graph.nodes.colors[e].r,
                                 graph.nodes.colors[e].g,
                                 graph.nodes.colors[e].b,
                                 GLubyte(graph.nodes.edgeSizes[e]),
                                 {graph.nodes.positions[e].x, graph.nodes.positions[e].y, graph.nodes.positions[e].z}};
    }

    // Orphan and then refill GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, m_edgeData.size() * sizeof(EdgeData), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_edgeData.size() * sizeof(EdgeData), m_edgeData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    globalLogger->info("Finish updating edges");
}

void Engine::updateSelectorBuffer() {
    m_picking->Begin();
    m_picking->pickingShader->Use();
    m_picking->pickingShader->SetUInt("nodeOffset", 0);

    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_graph->nodes.positions.size());
    m_picking->End();
}

void Engine::processMouseSelectorInput(GLFWwindow *window) {
    if (m_state == stop || m_gui->Active() || m_mouseActive) {
        if (m_hoveredNodeID >= 0) {
            m_previousHoveredNodeID = m_hoveredNodeID;
            m_inTransition = true;
            m_hoverBrightness = 0.0f;
        }
        m_hoveredNodeID = -1;
        return;
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    updateSelectorBuffer();

    int newHoveredNodeID = m_picking->ReadNodeID(static_cast<int>(xpos), static_cast<int>(ypos));

    if (newHoveredNodeID != m_hoveredNodeID) {
        if (m_hoveredNodeID >= 0) {
            m_previousHoveredNodeID = m_hoveredNodeID;
        }

        m_hoveredNodeID = newHoveredNodeID;
        m_inTransition = true;
        m_hoverBrightness = 0.0f;
    }
}

// Update node and edge buffers with new position data. Future optimisation to keep in mind: pack the position data
// together so it can be copied all at once as opposed to looping through each value.
void Engine::updateParticles(GS::Graph &graph) {
    uint32_t minNodeCount = std::min(m_nodeData.size(), graph.nodes.positions.size());
    for (uint32_t i = 0; i < minNodeCount; i++) {
        const auto &src = graph.nodes.positions.at(i);
        auto &dst = m_nodeData.at(i).position;

        dst[0] = src.x;
        dst[1] = src.y;
        dst[2] = src.z;
    }

    m_text->UpdateLabelPositions(graph.nodes.positions, graph.nodes.sizes);

    uint32_t minEdgeCount = std::min(m_edgeData.size() / 2, graph.edges.startIdxs.size());
    for (uint32_t i = 0; i < minEdgeCount; i++) {
        const uint32_t lineIdx = i * 2;

        auto EdgeStart = [&](size_t i) { return graph.nodes.positions.at(graph.edges.startIdxs.at(i)); };
        auto EdgeEnd = [&](size_t i) { return graph.nodes.positions.at(graph.edges.endIdxs.at(i)); };

        m_edgeData[lineIdx].position[0] = EdgeStart(i).x;
        m_edgeData[lineIdx].position[1] = EdgeStart(i).y;
        m_edgeData[lineIdx].position[2] = EdgeStart(i).z;

        m_edgeData[lineIdx + 1].position[0] = EdgeEnd(i).x;
        m_edgeData[lineIdx + 1].position[1] = EdgeEnd(i).y;
        m_edgeData[lineIdx + 1].position[2] = EdgeEnd(i).z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NodeData) * m_nodeData.size(), m_nodeData.data());

    globalLogger->critical(m_nodeData.size());

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_edgeData.size() * sizeof(EdgeData), m_edgeData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::computeHoverTransition(float deltaTime) {
    if (m_inTransition) {
        m_hoverBrightness += deltaTime * m_transitionSpeed;

        if (m_hoverBrightness >= 1.0f) {
            m_hoverBrightness = 1.0f;
            m_inTransition = false;

            if (m_hoveredNodeID < 0) {
                m_previousHoveredNodeID = -1;
            }
        }
    }
}

void Engine::computeLighting(glm::vec3 cameraPosition) {
    EnvironmentLighting uniforms = {};
    uniforms.globalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    uniforms.globalLightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    uniforms.pointLightCount = 3;

    uniforms.pointLight[0] = {cameraPosition, glm::vec3(1.0f, 1.0f, 1.0f), 0.006f, 0.013f, 0.089f};

    if (m_previousHoveredNodeID >= 0 && m_previousHoveredNodeID < static_cast<int>(m_graph->nodes.titles.size()) &&
        m_inTransition) {

        glm::vec3 prevNodePos = m_graph->nodes.positions[m_previousHoveredNodeID];
        glm::vec3 dirToCamera = cameraPosition - prevNodePos;
        dirToCamera = glm::normalize(dirToCamera);
        glm::vec3 prevLightPos = prevNodePos + (dirToCamera * 1.0f);

        uniforms.pointLight[1] = {prevLightPos, glm::vec3(1.0f - m_hoverBrightness), 0.001f, 0.3, 0.01};
    } else {
        uniforms.pointLight[1] = {glm::vec3(0), glm::vec3(0), 0.0f, 0.0f, 0.0f};
    }

    if (m_hoveredNodeID >= 0 && m_hoveredNodeID < static_cast<int>(m_graph->nodes.titles.size())) {
        m_gui->SetActiveNodeInfo(m_graph->nodes.titles[m_hoveredNodeID]);
        glm::vec3 nodePos = m_graph->nodes.positions[m_hoveredNodeID];
        glm::vec3 dirToCamera = cameraPosition - nodePos;
        dirToCamera = glm::normalize(dirToCamera);
        glm::vec3 lightPos = nodePos + (dirToCamera * 1.0f);

        uniforms.pointLight[2] = {lightPos, glm::vec3(m_hoverBrightness), 0.001f, 0.3, 0.01};
    } else {
        if (m_selectedNode >= 0 && m_selectedNode < static_cast<int32_t>(m_graph->nodes.titles.size())) {
            glm::vec3 nodePos = m_graph->nodes.positions[m_selectedNode];
            glm::vec3 dirToCamera = cameraPosition - nodePos;
            dirToCamera = glm::normalize(dirToCamera);
            glm::vec3 lightPos = nodePos + (dirToCamera * 1.0f);
            m_gui->SetActiveNodeInfo(m_graph->nodes.titles[m_selectedNode]);
            uniforms.pointLight[2] = {lightPos, glm::vec3(1.0), 0.001f, 0.3, 0.01};
        } else {
            m_gui->SetActiveNodeInfo("");
            uniforms.pointLight[2] = {glm::vec3(0), glm::vec3(0), 0.0f, 0.0f, 0.0f};
        }
    }

    m_shader.environmentUBO->Update(uniforms);
}

Engine::~Engine() {
    glDeleteVertexArrays(count, m_VAOs);
    glDeleteBuffers(count, m_VBOs);

    glfwTerminate();
}

uint32_t Engine::Run() {
    glfwSwapInterval(0);

    bool textureGenTriggered = false;
    std::future<LabelAtlasData> fut;

    while (!glfwWindowShouldClose(m_window)) {

        if (m_controlData.engine.initGraphData.load(std::memory_order_relaxed)) {
            m_controlData.engine.initGraphData.store(false, std::memory_order_relaxed);
            GS::Graph *graph = m_graphBuf.GetCurrent();

            updateNodes(*graph);
            updateEdges(*graph);

            fut = std::async(std::launch::async, [engine = m_text.get(), graph = graph->nodes.titles]() {
                return engine->PrepareLabelAtlases(graph);
            });
            textureGenTriggered = true;
        }

        // textureGenTriggered;
        if (fut.valid() && fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            LabelAtlasData atlasData = fut.get();
            m_text->UploadLabelAtlasesToGPU(atlasData);
            textureGenTriggered = false;
        }

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
    m_gui->RenderFPSWidget();

    processEngineInput(m_window);

    const float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (m_state == stop)
        deltaTime = 0;

    computeHoverTransition(deltaTime);

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

    m_camera.ProcessMovement(deltaTime);

    glm::vec3 cameraPosition = m_camera.GetCameraPosition();
    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 view = m_camera.GetViewMatrix();
    glm::mat3 normal = m_camera.CalcNormalMatrix();

    const CameraMatrices cameraMatrices{projection, view, glm::vec4(cameraPosition, 1.0)};
    const glm::mat4 cameraDirection = cameraMatrices.projection * glm::mat4(glm::mat3(view));

    computeLighting(cameraPosition);

    m_skybox->Display(cameraDirection);
    m_shader.cameraMatricesUBO->Update(cameraMatrices);

    processMouseSelectorInput(m_window);

    m_shader.sphere->Use();
    m_shader.sphere->SetFloat("time", currentFrame);

    MaterialProperties properties = {m_controlData.engine.customVals[0], m_controlData.engine.customVals[1],
                                     m_controlData.engine.customVals[2]};

    m_shader.materialUBO->Update(properties);

    if (m_hoveredNodeID >= 0 && m_hoveredNodeID < static_cast<int>(m_graph->nodes.titles.size())) {
        m_shader.sphere->SetInt("selectedID", m_hoveredNodeID);
    } else {
        m_shader.sphere->SetInt("selectedID", -1);
    }

    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_graph->nodes.titles.size());

    m_text->RenderLabels(currentFrame);

    m_shader.cylinder->Use();
    m_shader.cylinder->SetMat3("normalMat", normal);
    m_shader.cylinder->SetFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_LINES, 0, m_graph->edges.startIdxs.size() * 2);
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
    // m_text2d->UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));
    m_picking->Resize(width, height);

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

void Engine::doubleClickCalled() { m_controlData.graph.sourceNode.store(m_hoveredNodeID, std::memory_order_relaxed); }

void Engine::handleDoubleClick(int action) {
    static auto lastClickTime = std::chrono::system_clock::now();
    static bool waitingForSecondClick = false;
    static bool doubleClickProcessed = false;

    const double doubleClickThreshold = 400.0;

    if (action == GLFW_PRESS) {
        auto now = std::chrono::system_clock::now();
        double timeSinceLastClick = std::chrono::duration<double, std::milli>(now - lastClickTime).count();

        if (waitingForSecondClick && timeSinceLastClick <= doubleClickThreshold && !doubleClickProcessed) {

            doubleClickCalled();

            doubleClickProcessed = true;
            waitingForSecondClick = false;
        } else if (!waitingForSecondClick) {
            waitingForSecondClick = true;
            doubleClickProcessed = false;
        }

        lastClickTime = now;
    } else if (action == GLFW_RELEASE) {
        auto now = std::chrono::system_clock::now();
        double timeSinceLastClick = std::chrono::duration<double, std::milli>(now - lastClickTime).count();

        if (timeSinceLastClick > doubleClickThreshold) {
            waitingForSecondClick = false;
            doubleClickProcessed = false;
        }
    }
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
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS && !m_gui->Active()) {
            if (m_hoveredNodeID >= 0 && m_hoveredNodeID < static_cast<int>(m_graph->nodes.titles.size())) {
                m_selectedNode = m_hoveredNodeID;
            } else if (m_hoveredNodeID != m_selectedNode) {
                m_selectedNode = -1;
            }
        }
        handleDoubleClick(action);
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
