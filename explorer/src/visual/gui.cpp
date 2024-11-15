#include "./gui.hpp"

#include "../../lib/shader.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

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
    m_sphereShader = std::make_unique<Shader>("sphere.vert", "sphere.frag");

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

    std::vector<GLfloat> g_node_position_size_data = {1.0f, 1.2f, 0.8f, 1};
    std::vector<GLubyte> g_node_color_data = {244, 100, 100, 255};

    static const GLfloat quadVertices[] = {-0.5f, -0.5f, 0.0f, 0.0, 0.0, 0.5f, -0.5f, 0.0f, 0.0, 1.0,
                                           -0.5f, 0.5f,  0.0f, 1.0, 0.0, 0.5f, 0.5f,  0.0f, 1.0, 1.0};

    m_MaxNodes = 1;

    glBindVertexArray(m_VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // glBindBuffer(GL_ARRAY_BUFFER, node_position_buffer);
    // glBufferData(GL_ARRAY_BUFFER, m_MaxNodes * 4 * sizeof(GLfloat), &g_node_position_size_data.front(),
    // GL_STATIC_DRAW); glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0); glEnableVertexAttribArray(1);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, node_color_buffer);
    // glBufferData(GL_ARRAY_BUFFER, m_MaxNodes * 4 * sizeof(GLubyte), &g_node_color_data.front(), GL_STATIC_DRAW);
    // glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)0);
    // glEnableVertexAttribArray(2);

    // glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    // glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    // glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

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

    glBindVertexArray(m_VAOs[1]);

    glBindTexture(GL_TEXTURE_2D, m_sphereTexture);

    m_sphereShader->use();
    m_sphereShader->setMat4("PV", m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());

    glm::mat4 ViewMatrix = m_camera.GetViewMatrix();

    // m_sphereShader->setVec3("CameraRight_worldspace", ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    // m_sphereShader->setVec3("CameraUp_worldspace", ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_LINE_SMOOTH);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 position(0.2f, 1.0f, 0.7f);
    model = glm::translate(model, position);

    m_sphereShader->setMat4("model", model);

    // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_MaxNodes);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);
    // glDisableVertexAttribArray(2);

    //----------------------------

    // m_skybox->Display(m_camera);

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
