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

bool bloom = true;
float exposure = 1.0f;

Engine::Engine(const int &maxNodes, std::vector<Node> &nodes, std::vector<Edge> &lines) {
    m_nodes = nodes;

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
    glfwSetScrollCallback(m_window, scroll_callback_static);
    glfwSetCursorPosCallback(m_window, mouse_callback_static);

    m_camera.SetPosition(glm::vec3(3.0f, 0.0f, 0.0f), glm::pi<float>(), 0.0f);
    m_camera.SetAspectRatio(static_cast<float>(m_scrWidth) / static_cast<float>(m_scrHeight));

    m_skyboxShader = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_screenShaderBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_sphereShader = std::make_unique<Shader>("sphere.vert", "sphere.frag", "sphere.geom");
    m_lineShader = std::make_unique<Shader>("cylinder.vert", "cylinder.frag", "cylinder.geom");
    shaderBlur = std::make_unique<Shader>("blur.vert", "blur.frag");
    shaderBloomFinal = std::make_unique<Shader>("bloomFinal.vert", "bloomFinal.frag");

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, glm::ivec2(m_scrWidth, m_scrHeight),
                                            glm::ivec2(1000, 800), 100, true, 5.f, 15, 0.94f);

    m_text = std::make_unique<Text>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");
    m_text2d = std::make_unique<Text2d>("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", "text.vert", "text.frag");

    m_cameraMatricesUBO = std::make_unique<UBOManager<CameraMatrices>>(m_CAMERA_MATRICES_UBO_BINDING_POINT);
    m_environmentUBO = std::make_unique<UBOManager<EnvironmentLighting>>(m_ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);

    m_sphereShader->linkUBO("GlobalUniforms", m_CAMERA_MATRICES_UBO_BINDING_POINT);
    m_lineShader->linkUBO("GlobalUniforms", m_CAMERA_MATRICES_UBO_BINDING_POINT);
    m_text2d->m_textShader->linkUBO("GlobalUniforms", m_CAMERA_MATRICES_UBO_BINDING_POINT);
    m_text->m_textShader->linkUBO("GlobalUniforms", m_CAMERA_MATRICES_UBO_BINDING_POINT);

    m_sphereShader->linkUBO("EnvironmentUniforms", m_ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);
    m_lineShader->linkUBO("EnvironmentUniforms", m_ENVIRONMENT_LIGHTING_UBO_BINDING_POINT);

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
        points[i].r = nodes[i].rgb[0];
        points[i].g = nodes[i].rgb[1];
        points[i].b = nodes[i].rgb[2];
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
    m_lineCount = lines.size();

    struct EdgeData {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte radius;
        GLfloat position[3];
    };

    EdgeData edgeData[m_lineCount * 2];

    for (unsigned int i = 0; i < m_lineCount; i++) {
        const int lineIdx = i * 2;

        edgeData[lineIdx].r = lines[i].startRGB[0];
        edgeData[lineIdx].g = lines[i].startRGB[1];
        edgeData[lineIdx].b = lines[i].startRGB[2];
        edgeData[lineIdx].radius = static_cast<GLubyte>(lines[i].size);
        edgeData[lineIdx].position[0] = lines[i].start.x;
        edgeData[lineIdx].position[1] = lines[i].start.y;
        edgeData[lineIdx].position[2] = lines[i].start.z;

        edgeData[lineIdx + 1].r = lines[i].endRGB[0];
        edgeData[lineIdx + 1].g = lines[i].endRGB[1];
        edgeData[lineIdx + 1].b = lines[i].endRGB[2];
        edgeData[lineIdx + 1].radius = static_cast<GLubyte>(lines[i].size);
        edgeData[lineIdx + 1].position[0] = lines[i].end.x;
        edgeData[lineIdx + 1].position[1] = lines[i].end.y;
        edgeData[lineIdx + 1].position[2] = lines[i].end.z;
    }

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edgeData), edgeData, GL_STATIC_DRAW);

    const GLint radiusAttrib = m_lineShader->getAttribLocation("aRGBRadius");
    glEnableVertexAttribArray(radiusAttrib);
    glVertexAttribPointer(radiusAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(EdgeData), (void *)0);

    const GLint startAttrib = m_lineShader->getAttribLocation("aPos");
    glEnableVertexAttribArray(startAttrib);
    glVertexAttribPointer(startAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(EdgeData), (void *)(sizeof(float)));

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDepthFunc(GL_LESS);

    // m_blur->SetEnabled(false);
    m_text2d->UpdateScreenSize(static_cast<float>(m_scrWidth), static_cast<float>(m_scrHeight));

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, m_scrWidth, m_scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }  

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_scrWidth, m_scrHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);  
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_scrWidth, m_scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    shaderBlur->use();
    shaderBlur->setInt("image", 0);
    shaderBloomFinal->use();
    shaderBloomFinal->setInt("scene", 0);
    shaderBloomFinal->setInt("bloomBlur", 1);
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

int Engine::Run() {
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

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
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

    m_environmentUBO->Update(uniforms);

    m_skybox->Display(cameraDirection);

    m_cameraMatricesUBO->Update(cameraMatrices);

    m_sphereShader->use();
    m_sphereShader->setFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, m_nodeCount);

    m_lineShader->use();
    m_lineShader->setMat3("normalMat", normal);
    m_lineShader->setFloat("time", currentFrame);
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_LINES, 0, m_lineCount * 2);

    m_text->SetTransforms(view, currentFrame);

    for (Node node : m_nodes) {
        m_text->Render(node.text, node.pos, 0.004f, glm::vec3(1.0, 1.0f, 1.0f));
    }

    m_blur->Display();

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_state == pause) {
        m_text2d->Render2d(
            "WikiMapper",
            glm::vec3((static_cast<float>(m_scrWidth) * 0.5f), static_cast<float>(m_scrHeight) * 0.5f, 1.0f), 1.0f,
            glm::vec3(0.3, 0.7f, 0.9f));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;

    shaderBlur->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur->setInt("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderBloomFinal->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    shaderBloomFinal->setInt("bloom", bloom);
    shaderBloomFinal->setFloat("exposure", exposure);
    renderQuad();

}

void Engine::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

    m_scrWidth = width;
    m_scrHeight = height;
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
