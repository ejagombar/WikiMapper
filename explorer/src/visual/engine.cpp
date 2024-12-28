#include "./engine.hpp"

#include "../../lib/rgb_hsv.hpp"
#include "./shader.hpp"
#include "camera.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <algorithm>
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

Engine::Engine(const int &MaxNodes, std::vector<Node> &nodes, std::vector<glm::vec3> &lines) {
    m_nodes = nodes;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_ScrWidth, m_ScrHeight, "WikiMapper", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, m_ScrWidth, m_ScrHeight);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback_static);
    glfwSetKeyCallback(m_window, key_callback_static);
    glfwSetScrollCallback(m_window, scroll_callback_static);
    glfwSetCursorPosCallback(m_window, mouse_callback_static);

    m_camera.SetPosition();
    m_camera.SetAspectRatio(static_cast<float>(m_ScrWidth) / static_cast<float>(m_ScrHeight));

    m_skyboxShader = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_screenShaderBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_sphereShader = std::make_unique<Shader>("sphere.vert", "sphere.frag", "sphere.geom");
    m_lineShader = std::make_unique<Shader>("line.vert", "line.frag", "line.geom");

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, glm::ivec2(m_ScrWidth, m_ScrHeight),
                                            glm::ivec2(1000, 800), 100, true, 5.f, 15, 0.94f);

    m_text = std::make_unique<Text>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");
    m_text2d = std::make_unique<Text2d>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");

    m_globalUBO = std::make_unique<UBOManager<CameraMatrices>>(m_GLOBAL_UNIFORM_BINDING_POINT);
    m_EnvironmentUBO = std::make_unique<UBOManager<GlobalUniforms>>(m_LIGHTING_UBO);

    m_sphereShader->linkUBO("GlobalUniforms", m_GLOBAL_UNIFORM_BINDING_POINT);
    m_lineShader->linkUBO("GlobalUniforms", m_GLOBAL_UNIFORM_BINDING_POINT);
    m_text2d->m_textShader->linkUBO("GlobalUniforms", m_GLOBAL_UNIFORM_BINDING_POINT);
    m_text->m_textShader->linkUBO("GlobalUniforms", m_GLOBAL_UNIFORM_BINDING_POINT);

    m_sphereShader->linkUBO("EnvironmentUniforms", m_LIGHTING_UBO);

#if RecordCameraMovement
    std::remove("benchmarkCameraTrack");
    std::remove("benchmarkTimestamps");
    m_positionFile.open("benchmarkCameraTrack", std::ios::app | std::ios::binary);
    m_benchmarkTimestamps.open("benchmarkTimestamps", std::ios::app | std::ios::binary);
#endif
#if ReplayCameraMovement
    m_camPosData = ReadFileData<CameraPositionData>("benchmarkCameraTrack");
    std::reverse(m_camPosData.begin(),
                 m_camPosData.end()); // Inefficient, but doesn't matter as this runs before the benchmark starts
    m_benchmarkTimestampsData = ReadFileData<double>("benchmarkTimestamps");
    std::reverse(m_benchmarkTimestampsData.begin(), m_benchmarkTimestampsData.end());
#endif

    // -------------------- Texture -------------------------
    GLuint cubemapTexture = LoadCubemap(std::vector<std::string>{"stars.jpg"});

    // -----------------------------------------------------
    m_skybox = std::make_unique<Skybox>(*m_skyboxShader, cubemapTexture);

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    // Nodes -------------------------------------------------------------------
    struct NodeData {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte radius;
        GLfloat position[3];
    };

    m_nodeCount = nodes.size();
    NodeData points[m_nodeCount];

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (int i = 0; i < m_nodeCount; i++) {
        auto col = hsv2rgb(dist(gen), 1.0f, 1.0f);
        points[i].r = col.r;
        points[i].g = col.g;
        points[i].b = col.b;
        points[i].radius = static_cast<GLubyte>(nodes[i].size);
        points[i].position[0] = nodes[i].pos.x;
        points[i].position[1] = nodes[i].pos.y;
        points[i].position[2] = nodes[i].pos.z;
    }

    glBindVertexArray(m_VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    const GLint aColorAttrib = m_sphereShader->getAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(aColorAttrib);
    glVertexAttribPointer(aColorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NodeData), (void *)0);

    const GLint aPosAttrib = m_sphereShader->getAttribLocation("aPos");
    glEnableVertexAttribArray(aPosAttrib);
    glVertexAttribPointer(aPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(NodeData), (void *)(sizeof(float)));

    // Lines -------------------------------------------------------------------
    m_lineCount = lines.size() / 2;

    struct VertexData {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte radius;
        GLfloat position[6];
    };

    VertexData h_data[m_lineCount];

    for (unsigned int i = 0; i < m_lineCount; i++) {
        uint lineIdx = i * 2;
        auto col = hsv2rgb(dist(gen), 1.0f, 1.0f);

        h_data[i].r = col.r;
        h_data[i].g = col.g;
        h_data[i].b = col.b;
        h_data[i].radius = 1;

        h_data[i].position[0] = lines[lineIdx].x;
        h_data[i].position[1] = lines[lineIdx].y;
        h_data[i].position[2] = lines[lineIdx].z;

        h_data[i].position[3] = lines[lineIdx + 1].x;
        h_data[i].position[4] = lines[lineIdx + 1].y;
        h_data[i].position[5] = lines[lineIdx + 1].z;
    }

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(h_data), h_data, GL_STATIC_DRAW);

    const GLint radiusAttrib = m_lineShader->getAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(radiusAttrib);
    glVertexAttribPointer(radiusAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexData), (void *)0);

    const GLint startAttrib = m_lineShader->getAttribLocation("Start");
    glEnableVertexAttribArray(startAttrib);
    glVertexAttribPointer(startAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)(sizeof(float)));

    const GLint endAttrib = m_lineShader->getAttribLocation("End");
    glEnableVertexAttribArray(endAttrib);
    glVertexAttribPointer(endAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)(4 * sizeof(float)));

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    m_blur->SetEnabled(false);
    m_text2d->UpdateScreenSize(static_cast<float>(m_ScrWidth), static_cast<float>(m_ScrHeight));
}

Engine::~Engine() {
    glDeleteVertexArrays(count, m_VAOs);
    glDeleteBuffers(count, m_VBOs);

#if RecordCameraMovement
    m_positionFile.close();
#endif
#if ReplayCameraMovement or RecordCameraMovement
    m_benchmarkTimestamps.close();
#endif
    std::cout << "Execution Time: " << glfwGetTime() - m_startTime << std::endl;

    glfwTerminate();
}

int Engine::run() {
    double lastTime = glfwGetTime();
    m_startTime = lastTime;
    m_startFrameTime = lastTime;

#if ReplayCameraMovement
    glfwSwapInterval(0);
#endif

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();

        m_frameCount++;
#if !ReplayCameraMovement
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            printf("%f fps\n", double(m_frameCount));
            m_frameCount = 0;
            lastTime += 1.0;
        }
#endif

#if ReplayCameraMovement
        if (currentTime - m_startTime > m_benchmarkTimestampsData.back()) {

            if (m_benchmarkRecord) {
                std::cout << ((currentTime - m_startFrameTime) / m_frameCount) * 1000 << "," << std::flush;
                glfwSetWindowTitle(m_window, "Not Benchmarking");
            } else
                glfwSetWindowTitle(m_window, "Benchmarking");

            m_benchmarkRecord = !m_benchmarkRecord;
            m_frameCount = 0;

            m_startFrameTime = currentTime;
            m_benchmarkTimestampsData.pop_back();
            if (m_benchmarkTimestampsData.size() == 0)
                glfwSetWindowShouldClose(m_window, 1);
        }

        if (currentTime >= m_startTime + .1 * m_recordCount) {
            m_recordCount++;
            CameraPositionData p = m_camPosData.back();
            m_camera.SetPosition(p.position, p.yaw, p.pitch);
            m_camPosData.pop_back();
            m_lastCameraRecord = currentTime;
        }
#endif

#if RecordCameraMovement
        if (currentTime >= m_startTime + .1 * m_recordCount) {
            m_recordCount++;
            auto positionData = m_camera.GetPosition();
            m_positionFile.write((char *)&positionData, sizeof(positionData));
            m_lastCameraRecord = currentTime;
        }
#endif

        loop();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    return 0;
}

void Engine::loop() {
    processEngineInput(m_window);
    const float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (m_state == pause)
        deltaTime = 0;

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
    const glm::mat4 cameraDirection = cameraMatrices.Projection * glm::mat4(glm::mat3(view));

    GlobalUniforms uniforms = {};
    uniforms.GlobalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    uniforms.GlobalLightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    uniforms.NumPointLights = 2;

    uniforms.PointLights[0] = {cameraPosition, glm::vec3(1.0f, 0.5f, 0.5f), 1.0f, 0.09f, 0.032f};
    uniforms.PointLights[1] = {glm::vec3(-2.0f, 1.0f, -1.0f), glm::vec3(0.5f, 0.5f, 1.0f), 1.0f, 0.07f, 0.017f};

    m_EnvironmentUBO->Update(uniforms);

    m_skybox->Display(cameraDirection);

    m_globalUBO->Update(cameraMatrices);

    m_sphereShader->use();
    m_sphereShader->setVec3("LightPosition", cameraPosition);
    m_sphereShader->setVec3("LightColor", glm::vec3(0.8f, 0.8f, 0.8f));
    m_sphereShader->setVec3("GlobalLightColor", glm::vec3(0.7f, 0.8f, 0.8f));
    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_nodeCount);

    m_lineShader->use();
    m_lineShader->setVec4("lightPos", glm::vec4(0.8f, 4.8f, 5.8f, 1.0f));
    m_lineShader->setMat3("NormalMat", normal);
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_POINTS, 0, m_lineCount);

    m_text->SetTransforms(view);

    for (Node node : m_nodes) {
        m_text->Render(node.text, node.pos, 0.004f, glm::vec3(1.0, 1.0f, 1.0f));
    }

    m_blur->Display();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_state == pause) {
        m_text2d->Render(
            "WikiMapper",
            glm::vec3((static_cast<float>(m_ScrWidth) * 0.5f), static_cast<float>(m_ScrHeight) * 0.5f, 1.0f), 1.0f,
            glm::vec3(0.3, 0.7f, 0.9f));
    }
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

void Engine::scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset) {
    Engine *instance = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->scroll_callback(window, xoffset, yoffset);
    }
}

void Engine::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    m_camera.ProcessMouseScroll(yoffset);
}

void Engine::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    m_blur->ScreenResize(glm::ivec2(width, height));
    m_text2d->UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));

    m_ScrWidth = width;
    m_ScrHeight = height;
}

void Engine::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
#if !ReplayCameraMovement
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
#endif
}

void Engine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
#if !ReplayCameraMovement
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if (m_state == play) {
            m_state = pause;
            m_blur->SetEnabled(true);

            glfwSetCursorPosCallback(m_window, NULL);
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            m_state = play;
            m_blur->SetEnabled(false);

            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_firstMouse = true;
            glfwSetCursorPosCallback(m_window, mouse_callback_static);
        }
    }
#endif
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        double time = glfwGetTime() - m_startTime;
        std::cout << m_benchmarkRecord << std::endl;
#if RecordCameraMovement
        if (m_benchmarkRecord)
            m_benchmarkTimestamps.write((char *)&time, sizeof(time));
#endif
        glfwSetWindowShouldClose(m_window, 1);
    }
#if RecordCameraMovement
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        double time = glfwGetTime() - m_startTime;
        m_benchmarkTimestamps.write((char *)&time, sizeof(time));
        if (m_benchmarkRecord) {
            glfwSetWindowTitle(m_window, "Not recording");
        } else
            glfwSetWindowTitle(m_window, "Recording");
        m_benchmarkRecord = !m_benchmarkRecord;
    }
#endif
}

void Engine::processEngineInput(GLFWwindow *window) {
#if !ReplayCameraMovement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.ProcessKeyboard(UP);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_camera.ProcessKeyboard(DOWN);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_camera.ProcessKeyboard(SNEAK);
#endif
}
