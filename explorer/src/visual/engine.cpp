#include "./engine.hpp"

#include "./shader.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

const int COUNT = 42;

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

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, glm::ivec2(m_ScrWidth, m_ScrHeight),
                                            glm::ivec2(1000, 800), 100, true, 5.f, 15, 0.94f);

    m_text = std::make_unique<Text>("/usr/share/fonts/FiraCode/FiraCodeNerdFont-Medium.ttf", "text.vert", "text.frag");
    m_text2d =
        std::make_unique<Text2d>("/usr/share/fonts/FiraCode/FiraCodeNerdFont-Medium.ttf", "text.vert", "text.frag");

    // -------------------- Texture -------------------------
    GLuint cubemapTexture = LoadCubemap(std::vector<std::string>{"stars.jpg"});

    // -----------------------------------------------------
    m_skybox = std::make_unique<Skybox>(*m_skyboxShader, cubemapTexture);

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    // Nodes -------------------------------------------------------------------
    float points[COUNT * 7]; // Pos(XYZ), Col(RGB), Size(R)

    std::cout << "MaxNodes: " << MaxNodes << std::endl;

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_int_distribution<> dist{-100, 100};
    for (int i = 0; i < COUNT; i++) {
        int j = i * 7;
        points[j] = nodes[i].pos.x;
        points[j + 1] = nodes[i].pos.y;
        points[j + 2] = nodes[i].pos.z;
        points[j + 3] = nodes[i].r / 255.0f;
        points[j + 4] = nodes[i].g / 255.0f;
        points[j + 5] = nodes[i].b / 255.0f;
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

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * 3 * sizeof(float), &lines.front(), GL_STATIC_DRAW);

    std::cout << "Length: " << lines.size() << std::endl;

    const GLint positionAttrib = m_lineShader->getAttribLocation("position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

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

    glfwTerminate();
}

int GUI::run() {
    int nbFrames = 0;
    double lastTime = glfwGetTime();
    // glfwSwapInterval(0);

    while (!glfwWindowShouldClose(m_window)) {
        nbFrames++;
        if (glfwGetTime() - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            printf("%f fps\n", double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

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
    m_sphereShader->setVec3("GlobalLightColor", glm::vec3(0.8f, 0.8f, 0.8f));

    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, COUNT);

    m_lineShader->use();
    m_lineShader->setMat4("Projection", m_camera.GetProjectionMatrix());
    m_lineShader->setMat4("View", m_camera.GetViewMatrix());
    m_lineShader->setVec3("CameraPosition", m_camera.GetCameraPosition());
    glBindVertexArray(m_VAOs[0]);
    glDrawArrays(GL_LINES, 0, 158);

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
        m_text2d->Render(
            "WikiMapper",
            glm::vec3((static_cast<float>(m_ScrWidth) * 0.5f), static_cast<float>(m_ScrHeight) * 0.5f, 1.0f), 1.0f,
            glm::vec3(0.3, 0.7f, 0.9f));
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

void GUI::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
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
}

void GUI::processEngineInput(GLFWwindow *window) {
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
}
