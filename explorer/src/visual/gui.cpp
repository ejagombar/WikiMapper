#include "./gui.hpp"

#include "../../lib/shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <vector>

int gui::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_SCR_WIDTH, m_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
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
    m_grassShader = std::make_unique<Shader>("grass.vert", "grass.frag");
    m_lightShader = std::make_unique<Shader>("lightshader.vert", "lightshader.frag");
    m_skyboxShader = std::make_unique<Shader>("skybox.vert", "skybox.frag");
    m_screenShaderBlur = std::make_unique<Shader>("framebuffer.vert", "framebufferblur.frag");
    m_screenShaderMix = std::make_unique<Shader>("framebuffer.vert", "framebuffermixer.frag");

    m_blur = std::make_unique<Filter::Blur>(*m_screenShaderBlur, *m_screenShaderMix, m_SCR_WIDTH, m_SCR_HEIGHT);

    // -------------------- Texture -------------------------
    m_grassTexture = LoadTexture("grass.png");

    // std::vector<std::string> faces = {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};
    std::vector<std::string> faces = {"stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg"};
    unsigned int cubemapTexture = LoadCubemap(faces);

    // -----------------------------------------------------
    m_skybox = std::make_unique<Skybox>(*m_skyboxShader, cubemapTexture);

    float vertices[] = {
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

    glGenVertexArrays(count, m_VAOs);
    glGenBuffers(count, m_VBOs);

    glBindVertexArray(m_VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    float planeVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

    glBindVertexArray(m_VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnable(GL_DEPTH_TEST);

    m_shader->use();
    m_shader->setInt("texture1", 0);

    m_grassShader->use();
    m_grassShader->setInt("texture1", 0);

    m_cubePositions = {glm::vec3(3.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                       glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                       glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                       glm::vec3(-1.3f, 1.0f, -1.5f)};

    glGenVertexArrays(1, &m_VAOs[2]);
    glBindVertexArray(m_VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    m_vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    m_vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    m_vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    m_vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    m_vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    m_blur->SetEnabled(false);

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

        engine_loop();
        if (m_state == play) [[likely]] {
        } else {
        }

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(count, m_VAOs);
    glDeleteBuffers(count, m_VBOs);

    glfwTerminate();
    return 0;
}

void gui::engine_loop() {

    processEngineInput(m_window);
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_blur->Preprocess();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_camera.ProcessPosition(deltaTime);

    // Grass ---------------------
    m_grassShader->use();
    glBindVertexArray(m_VAOs[1]);
    glBindTexture(GL_TEXTURE_2D, m_grassTexture);
    m_grassShader->setMat4("PV", m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());
    for (unsigned int i = 0; i < m_vegetation.size(); i++) {
        glm::mat4 grassModel = glm::mat4(1.0f);
        grassModel = glm::translate(grassModel, m_vegetation[i]);
        m_grassShader->setMat4("model", grassModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Light source --------------
    m_lightShader->use();
    glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
                                       glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

    m_lightShader->setMat4("PV", m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());
    glBindVertexArray(m_VAOs[2]);
    for (int i = 0; i < 4; i++) {
        glm::mat4 modelLightSource = glm::mat4(1.0f);
        modelLightSource = glm::translate(modelLightSource, pointLightPositions[i]);
        modelLightSource = glm::scale(modelLightSource, glm::vec3(0.2f));
        m_lightShader->setMat4("model", modelLightSource);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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
    m_shader->setVec3("pointLights[0].position", pointLightPositions[0]);
    m_shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    m_shader->setFloat("pointLights[0].constant", 1.0f);
    m_shader->setFloat("pointLights[0].linear", 0.09f);
    m_shader->setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    m_shader->setVec3("pointLights[1].position", pointLightPositions[1]);
    m_shader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    m_shader->setFloat("pointLights[1].constant", 1.0f);
    m_shader->setFloat("pointLights[1].linear", 0.09f);
    m_shader->setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    m_shader->setVec3("pointLights[2].position", pointLightPositions[2]);
    m_shader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    m_shader->setFloat("pointLights[2].constant", 1.0f);
    m_shader->setFloat("pointLights[2].linear", 0.09f);
    m_shader->setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    m_shader->setVec3("pointLights[3].position", pointLightPositions[3]);
    m_shader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    m_shader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    m_shader->setFloat("pointLights[3].constant", 1.0f);
    m_shader->setFloat("pointLights[3].linear", 0.09f);
    m_shader->setFloat("pointLights[3].quadratic", 0.032f);

    for (unsigned int i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_skybox->Display(m_camera);

    m_blur->Display();
}

void gui::key_callback_static(GLFWwindow *window, int key, int scancode, int action, int mods) {
    gui *instance = static_cast<gui *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->key_callback(window, key, scancode, action, mods);
    }
}

void gui::framebuffer_size_callback_static(GLFWwindow *window, int width, int height) {
    gui *instance = static_cast<gui *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->framebuffer_size_callback(window, width, height);
    }
}

void gui::mouse_callback_static(GLFWwindow *window, double xpos, double ypos) {
    gui *instance = static_cast<gui *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->mouse_callback(window, xpos, ypos);
    }
}

void gui::scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset) {
    gui *instance = static_cast<gui *>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->scroll_callback(window, xoffset, yoffset);
    }
}

void gui::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) { m_camera.ProcessMouseScroll(yoffset); }

void gui::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}

void gui::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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

void gui::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        std::cout << "dsffsdfsd" << std::endl;
        if (m_state == play) {
            m_state = pause;
            m_blur->SetEnabled(true);
        } else {
            m_state = play;
            m_blur->SetEnabled(false);
        }
    }
}

void gui::processEngineInput(GLFWwindow *window) {
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
