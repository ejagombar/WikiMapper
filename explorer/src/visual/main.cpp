#include <cstdint>
#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "../../lib/camera.hpp"
#include "../../lib/shader.hpp"
#include "../../lib/skybox.hpp"
#include "./filter.hpp"
#include <GL/gl.h> // This header isn't required as glad already provides it, however if it is not here, then the the language server automatically adds it when autocomplete is used on a OpenGL function
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/texture.hpp"

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float lastX;
float lastY;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Camera camera;

void print(std::string str) { std::cout << str << std::endl; }

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    camera.SetPosition();
    camera.SetAspectRatio(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));

    Shader shader("shader.vert", "shader.frag");
    Shader grassShader("grass.vert", "grass.frag");
    Shader lightShader("lightshader.vert", "lightshader.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader screenShaderBlur("framebuffer.vert", "framebufferblur.frag");
    Shader screenShaderMix("framebuffer.vert", "framebuffermixer.frag");

    Filter::Blur blur(screenShaderBlur, screenShaderMix, SCR_WIDTH, SCR_HEIGHT);

    // -------------------- Texture -------------------------
    unsigned int grassTexture = LoadTexture("grass.png");

    // std::vector<std::string> faces = {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};
    std::vector<std::string> faces = {"stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg", "stars.jpg"};
    unsigned int cubemapTexture = LoadCubemap(faces);

    // -----------------------------------------------------
    Skybox skybox(skyboxShader, cubemapTexture);

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

    const uint8_t count = 3;
    unsigned int VAOs[count], VBOs[count];
    glGenVertexArrays(count, VAOs);
    glGenBuffers(count, VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
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

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnable(GL_DEPTH_TEST);

    shader.use();
    shader.setInt("texture1", 0);

    grassShader.use();
    grassShader.setInt("texture1", 0);

    glm::mat4 modelLightSource = glm::mat4(1.0f);

    glm::vec3 cubePositions[] = {glm::vec3(3.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    // blur.SetEnabled(true);

    double lastTime2 = glfwGetTime();
    int nbFrames = 0;
    // glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        double currentTime2 = glfwGetTime();
        nbFrames++;
        if (currentTime2 - lastTime2 >= 1.0) { // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("%f fps\n", double(nbFrames));
            nbFrames = 0;
            lastTime2 += 1.0;
        }

        processInput(window);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        blur.Preprocess();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        camera.ProcessPosition(deltaTime);

        // Grass ---------------------
        grassShader.use();
        glBindVertexArray(VAOs[1]);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        grassShader.setMat4("PV", camera.GetProjectionMatrix() * camera.GetViewMatrix());
        for (unsigned int i = 0; i < vegetation.size(); i++) {
            glm::mat4 grassModel = glm::mat4(1.0f);
            grassModel = glm::translate(grassModel, vegetation[i]);
            grassShader.setMat4("model", grassModel);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Light source --------------
        lightShader.use();
        glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
                                           glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

        lightShader.setMat4("PV", camera.GetProjectionMatrix() * camera.GetViewMatrix());
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; i++) {
            modelLightSource = glm::mat4(1.0f);
            modelLightSource = glm::translate(modelLightSource, pointLightPositions[i]);
            modelLightSource = glm::scale(modelLightSource, glm::vec3(0.2f));
            lightShader.setMat4("model", modelLightSource);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Cubes -------------------
        shader.use();
        glBindVertexArray(VAOs[0]);
        shader.setVec3("viewPos", camera.GetCameraPosition());
        shader.setMat4("PV", camera.GetProjectionMatrix() * camera.GetViewMatrix());
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);

        shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);

        // directional light
        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 0.09f);
        shader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[2].constant", 1.0f);
        shader.setFloat("pointLights[2].linear", 0.09f);
        shader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[3].constant", 1.0f);
        shader.setFloat("pointLights[3].linear", 0.09f);
        shader.setFloat("pointLights[3].quadratic", 0.032f);

        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        skybox.Display(camera);

        blur.Display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(count, VAOs);
    glDeleteBuffers(count, VBOs);

    glfwTerminate();
    return 0;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) { camera.ProcessMouseScroll(yoffset); }

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // paused = !paused;
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN);
}
