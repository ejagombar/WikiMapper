#include "./gui.hpp"

#include "../../lib/shader.hpp"
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

const int COUNT = 200000;

GUI::GUI(const int &MaxNodes, std::vector<Node> &nodes) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_SCR_WIDTH, m_SCR_HEIGHT, "WikiMapper", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, m_SCR_WIDTH, m_SCR_HEIGHT);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback_static);
    glfwSetKeyCallback(m_window, key_callback_static);
    glfwSetScrollCallback(m_window, scroll_callback_static);
    glfwSetCursorPosCallback(m_window, mouse_callback_static);

    m_camera.SetPosition();
    m_camera.SetAspectRatio(static_cast<float>(m_SCR_WIDTH) / static_cast<float>(m_SCR_HEIGHT));

    m_shader = std::make_unique<Shader>("shader.vert", "shader.frag");
    m_skyboxShader = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_screenShaderBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_sphereShader = std::make_unique<Shader>("sphere.vert", "sphere.frag", "sphere.geom");

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, glm::ivec2(m_SCR_WIDTH, m_SCR_HEIGHT),
                                            glm::ivec2(1000, 800), 100, true, 5.f, 15, 0.94f);

    // -------------------- Texture -------------------------
    GLuint cubemapTexture = LoadCubemap(std::vector<std::string>{"stars.jpg"});
    m_sphereTexture = LoadTexture("sphere.png");

    // -----------------------------------------------------
    m_skybox = std::make_unique<Skybox>(*m_skyboxShader, cubemapTexture);

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    // -------------------------------------------------------------------
    static const GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,
        0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f,
        -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
        0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,
        -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,
        0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
        -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
        -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f,
        -0.5f, 0.0f,  -1.0f, 0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
        1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // -------------------------------------------------------------------

    float points[COUNT * 7]; // Pos(XYZ), Col(RGB), Size(R)

    std::cout << "MaxNodes: " << MaxNodes << std::endl;

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_int_distribution<> dist{-100, 100};
    for (int i = 0; i < COUNT; i++) {
        int j = i * 7;
        points[j] = dist(gen);
        points[j + 1] = dist(gen);
        points[j + 2] = dist(gen);
        points[j + 3] = 1.0f;
        points[j + 4] = 1.0f;
        points[j + 5] = 1.0f;
        points[j + 6] = 1.0f;
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

    // -------------------------------------------------------------------
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    m_shader->use();
    m_shader->setInt("texture1", 0);

    m_cubePositions = {glm::vec3(3.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                       glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                       glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                       glm::vec3(-1.3f, 1.0f, -1.5f)};

    m_blur->SetEnabled(false);
}

GUI::~GUI() {
    glDeleteVertexArrays(count, m_VAOs);
    glDeleteBuffers(count, m_VBOs);

    glfwTerminate();
}

int GUI::run() {
    int nbFrames = 0;
    double lastTime = glfwGetTime();
    glfwSwapInterval(0);

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

    // Cubes -------------------
    m_shader->use();
    glBindVertexArray(m_VAOs[0]);
    m_shader->setVec3("viewPos", m_camera.GetCameraPosition());
    m_shader->setMat4("PV", m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());
    m_shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);

    m_shader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    m_shader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    m_shader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    m_shader->setFloat("material.shininess", 32.0f);

    // directional light
    m_shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    m_shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    m_shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    m_shader->setVec3("pointLights[0].position", glm::vec3(0.7f, 0.2f, 2.0f));
    m_shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    m_shader->setFloat("pointLights[0].constant", 1.0f);
    m_shader->setFloat("pointLights[0].linear", 0.09f);
    m_shader->setFloat("pointLights[0].quadratic", 0.032f);

    for (unsigned int i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle) * sin(currentFrame), glm::vec3(1.0f, 0.3f, 0.5f));
        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    //------------------------------------------------------------------------------------------
    glm::mat4 camera_direction = m_camera.GetProjectionMatrix() * glm::mat4(glm::mat3(m_camera.GetViewMatrix()));
    m_skybox->Display(camera_direction);

    // -----------------------------
    m_sphereShader->use();
    glBindVertexArray(m_VAOs[1]);
    glDrawArrays(GL_POINTS, 0, COUNT);
    m_sphereShader->setMat4("Projection", m_camera.GetProjectionMatrix());
    m_sphereShader->setMat4("View", m_camera.GetViewMatrix());

    m_sphereShader->setVec3("CameraPosition", m_camera.GetCameraPosition());

    m_sphereShader->setVec3("LightPosition", glm::vec3(5.0f, 5.0f, 5.0f));

    m_blur->Display();
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
