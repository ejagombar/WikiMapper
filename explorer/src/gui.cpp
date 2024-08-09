#include "gui.h"
// #include <lib/controls.hpp>
// #include <lib/texture.hpp>
#include "../lib/shader.h"
#include "../lib/texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

using namespace glm;

gui::gui(const int &MaxNodes, std::vector<glm::vec3> &lines, std::vector<Node> &nodes)
    : m_MaxNodes(MaxNodes), m_lines(lines), m_nodes(nodes) {
    g_node_position_size_data.resize(m_MaxNodes * 4);
    g_node_color_data.resize(m_MaxNodes * 4);
}

int gui::initWindow() {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, "WikiMapper Explorer", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    // Allow the capture of the escape key
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, windowWidth / 2.0f, windowHeight / 2.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return 0;
}

void gui::loop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    lastTime = currentTime;

    camera.computeMatricesFromInputs(window);
    glm::mat4 ProjectionMatrix = camera.getProjectionMatrix();
    glm::mat4 ViewMatrix = camera.getViewMatrix();

    glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

    glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

    // Simulate all particles
    for (int i = 0; i < m_MaxNodes; i++) {
        Node &p = m_nodes[i]; // shortcut

        p.cameradistance = glm::length2(p.pos - CameraPosition);

        // Fill the GPU buffer
        g_node_position_size_data[4 * i + 0] = p.pos.x;
        g_node_position_size_data[4 * i + 1] = p.pos.y;
        g_node_position_size_data[4 * i + 2] = p.pos.z;

        g_node_position_size_data[4 * i + 3] = p.size;

        g_node_color_data[4 * i + 0] = p.r;
        g_node_color_data[4 * i + 1] = p.g;
        g_node_color_data[4 * i + 2] = p.b;
        g_node_color_data[4 * i + 3] = p.a;
    }

    glBindBuffer(GL_ARRAY_BUFFER, node_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_MaxNodes * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nodes.size() * sizeof(GLfloat),
                    &g_node_position_size_data.front());

    glBindBuffer(GL_ARRAY_BUFFER, node_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_MaxNodes * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nodes.size() * sizeof(GLubyte),
                    &g_node_color_data.front());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glUniform1i(TextureID, 0);

    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
    vec3 lightPos = CameraPosition;
    vec3 lightColor = vec3(1, 1, 1);
    int lightPower = 4;

    glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y,
                lightPos.z);
    glUniform3f(glGetUniformLocation(programID, "LightColor"), lightColor.x, lightColor.y,
                lightColor.z);
    glUniform1f(glGetUniformLocation(programID, "LightPower"), lightPower);
    glUniform3f(glGetUniformLocation(programID, "ViewPosition_worldspace"), CameraPosition.x,
                CameraPosition.y, CameraPosition.z);

    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, node_position_buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, node_color_buffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)0);

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_nodes.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // ---------------------------- LINES ----------------------------
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(VP2, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix));

    // Draw the line
    glBindVertexArray(VAO);
    glLineWidth(2.0f); // Set line thickness to 5
    glDrawArrays(GL_LINES, 0, m_lines.size());
    glBindVertexArray(4);
    // ---------------------------- LINES ----------------------------

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int gui::init() {
    if (initWindow() == -1)
        return -1;

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    programID = LoadShaders("nodeVertexShader.glsl", "nodeFragmentShader.glsl");

    // Vertex shader
    CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    // fragment shader
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    for (int i = 0; i < m_MaxNodes; i++) {
        m_nodes[i].cameradistance = -1.0f;
    }

    Texture = loadDDS("sphere512.DDS");

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f,
    };

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &node_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, node_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, m_nodes.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &node_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, node_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, m_nodes.size() * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    // ---------------------------- LINES ----------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(glm::vec3), &m_lines.front(),
                 GL_STATIC_DRAW);

    // Vertex attribute for position
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 4);
    glBindVertexArray(4);

    // Create shader program
    shaderProgram = LoadShaders("lineVertexShader.glsl", "lineFragmentShader.glsl");
    VP2 = glGetUniformLocation(shaderProgram, "VP");

    // ---------------------------- LINES ----------------------------

    lastTime = glfwGetTime();

    double lastTime2 = glfwGetTime();
    int nbFrames = 0;
    glfwSwapInterval(0);
    do {
        double currentTime2 = glfwGetTime();
        nbFrames++;
        if (currentTime2 - lastTime2 >= 1.0) { // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame\n", double(nbFrames));
            nbFrames = 0;
            lastTime2 += 1.0;
        }
        loop();
    } while (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &node_color_buffer);
    glDeleteBuffers(1, &node_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();

    return 0;
}
