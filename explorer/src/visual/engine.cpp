#include "./engine.hpp"

#include "../../lib/rgb_hsv.hpp"
#include "./shader.hpp"
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

float mrand(float a, float b) {
    float r = (float)rand() / RAND_MAX;
    return (b - a) * r + a;
}

GUI::GUI(const int &MaxNodes, std::vector<Node> &nodes, std::vector<glm::vec3> &lines) {
    m_nodes = nodes;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, glm::ivec2(m_ScrWidth, m_ScrHeight), glm::ivec2(1000, 800), 100, true, 5.f, 15, 0.94f);

    m_text = std::make_unique<Text>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");
    m_text2d = std::make_unique<Text2d>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");

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
    m_nodeCount = nodes.size();
    float points[m_nodeCount * 7]; // Pos(XYZ), Col(RGB), Size(R)

    std::cout << "MaxNodes: " << MaxNodes << std::endl;

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_int_distribution<> dist{-100, 100};
    std::uniform_real_distribution<> dist2{0, 1};
    for (int i = 0; i < m_nodeCount; i++) {
        auto col = hsv2rgb(mrand(0, 1), 1.0f, 1.0f);

        int j = i * 7;
        points[j] = nodes[i].pos.x;
        points[j + 1] = nodes[i].pos.y;
        points[j + 2] = nodes[i].pos.z;
        points[j + 3] = col.r;
        points[j + 4] = col.g;
        points[j + 5] = col.b;
        points[j + 6] = nodes[i].size;
    }

    glBindVertexArray(m_VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    const GLint aPosAttrib = m_sphereShader->getAttribLocation("aPos");
    glEnableVertexAttribArray(aPosAttrib);
    glVertexAttribPointer(aPosAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);

    const GLint aColorAttrib = m_sphereShader->getAttribLocation("aColor");
    glEnableVertexAttribArray(aColorAttrib);
    glVertexAttribPointer(aColorAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));

    const GLint aSizeAttrib = m_sphereShader->getAttribLocation("aSize");
    glEnableVertexAttribArray(aSizeAttrib);
    glVertexAttribPointer(aSizeAttrib, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(6 * sizeof(float)));

    // Lines -------------------------------------------------------------------

    const float RADIUS_MEAN = 0.06f;
    const float RADIUS_VAR = 0.08f;

    m_lineCount = lines.size() / 2;

    GLfloat h_data[10 * m_lineCount];

    for (unsigned int i = 0; i < m_lineCount; i++) {
        uint lineIdx = i * 2;
        uint h_dataIdx = i * 10;

        // start
        h_data[h_dataIdx] = lines[lineIdx].x;     // vertex.x
        h_data[h_dataIdx + 1] = lines[lineIdx].y; // vertex.y
        h_data[h_dataIdx + 2] = lines[lineIdx].z; // vertex.z

        // end
        h_data[h_dataIdx + 3] = lines[lineIdx + 1].x; // vertex.x
        h_data[h_dataIdx + 4] = lines[lineIdx + 1].y; // vertex.y
        h_data[h_dataIdx + 5] = lines[lineIdx + 1].z; // vertex.z

        // color
        auto col = hsv2rgb(mrand(0, 1), 1.0f, 1.0f);
        h_data[h_dataIdx + 6] = col.r; // Red
        h_data[h_dataIdx + 7] = col.g; // Green
        h_data[h_dataIdx + 8] = col.b; // Blue

        // radius
        h_data[h_dataIdx + 9] = RADIUS_VAR * rand() / RAND_MAX + RADIUS_MEAN;
    }

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(h_data), h_data, GL_STATIC_DRAW);

    std::cout << "Length: " << sizeof(h_data) << std::endl;

    const GLint startAttrib = m_lineShader->getAttribLocation("Start");
    glEnableVertexAttribArray(startAttrib);
    glVertexAttribPointer(startAttrib, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)0);

    const GLint endAttrib = m_lineShader->getAttribLocation("End");
    glEnableVertexAttribArray(endAttrib);
    glVertexAttribPointer(endAttrib, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(3 * sizeof(float)));

    const GLint colorAttrib = m_lineShader->getAttribLocation("Color");
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(6 * sizeof(float)));

    const GLint radiusAttrib = m_lineShader->getAttribLocation("Radius");
    glEnableVertexAttribArray(radiusAttrib);
    glVertexAttribPointer(radiusAttrib, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(9 * sizeof(float)));

    // -------------------------------------

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    m_blur->SetEnabled(false);
    m_text2d->UpdateScreenSize(static_cast<float>(m_ScrWidth), static_cast<float>(m_ScrHeight));
}

GUI::~GUI() {
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

int GUI::run() {
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

void GUI::loop() {
    processEngineInput(m_window);
    float currentFrame = static_cast<float>(glfwGetTime());
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

    //------------------------------------------------------------------------------------------
    glm::mat4 camera_direction = m_camera.GetProjectionMatrix() * glm::mat4(glm::mat3(m_camera.GetViewMatrix()));
    m_skybox->Display(camera_direction);

    // -----------------------------
    m_sphereShader->use();
    m_sphereShader->setMat4("Projection", m_camera.GetProjectionMatrix());
    m_sphereShader->setMat4("View", m_camera.GetViewMatrix());

    m_sphereShader->setVec3("CameraPosition", m_camera.GetCameraPosition());
    m_sphereShader->setVec3("LightPosition", m_camera.GetCameraPosition());
    m_sphereShader->setVec3("LightColor", glm::vec3(0.8f, 0.8f, 0.8f));
    m_sphereShader->setVec3("GlobalLightColor", glm::vec3(0.7f, 0.8f, 0.8f));

    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_nodeCount);

    m_lineShader->use();
    m_lineShader->setMat4("PMatrix", m_camera.GetProjectionMatrix());
    m_lineShader->setMat4("MVMatrix", m_camera.GetViewMatrix());
    m_lineShader->setVec4("EyePoint", glm::vec4(m_camera.GetCameraPosition(), 1.0f));
    m_lineShader->setVec4("lightPos", glm::vec4(0.8f, 4.8f, 5.8f, 1.0f));
    m_lineShader->setMat3("NormalMatrix", m_camera.GetNormalMatrix());
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_POINTS, 0, m_lineCount);

    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 View = m_camera.GetViewMatrix();

    m_text->SetTransforms(projection, View, m_camera.GetCameraPosition());

    for (Node node : m_nodes) {
        m_text->Render(node.text, node.pos, 0.004f, glm::vec3(1.0, 1.0f, 1.0f));
    }

    m_blur->Display();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_state == pause) {
        m_text2d->Render("WikiMapper", glm::vec3((static_cast<float>(m_ScrWidth) * 0.5f), static_cast<float>(m_ScrHeight) * 0.5f, 1.0f), 1.0f, glm::vec3(0.3, 0.7f, 0.9f));
    }
}

void GUI::key_callback_static(GLFWwindow *window, int key, int scancode, int action, int mods) {
    GUI *instance = static_cast<GUI *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->key_callback(window, key, scancode, action, mods);
    }
}

void GUI::framebuffer_size_callback_static(GLFWwindow *window, int width, int height) {
    GUI *instance = static_cast<GUI *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->framebuffer_size_callback(window, width, height);
    }
}

void GUI::mouse_callback_static(GLFWwindow *window, double xpos, double ypos) {
    GUI *instance = static_cast<GUI *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->mouse_callback(window, xpos, ypos);
    }
}

void GUI::scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset) {
    GUI *instance = static_cast<GUI *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->scroll_callback(window, xoffset, yoffset);
    }
}

void GUI::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) { m_camera.ProcessMouseScroll(yoffset); }

void GUI::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    m_blur->ScreenResize(glm::ivec2(width, height));
    m_text2d->UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));

    m_ScrWidth = width;
    m_ScrHeight = height;
}

void GUI::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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

void GUI::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

void GUI::processEngineInput(GLFWwindow *window) {
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
#endif
}
