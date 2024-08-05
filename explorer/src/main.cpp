#include "neo4j.h"
#include <httplib.h>

#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow *window;

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

// #include <lib/controls.hpp>
// #include <lib/texture.hpp>
#include "../lib/controls.hpp"
#include "../lib/shader.hpp"
#include "../lib/texture.hpp"

using namespace glm;

struct Node {
    glm::vec3 pos, speed;
    unsigned char r, g, b, a; // Color
    float size, angle, weight;
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Node &that) const {
        // Sort in reverse order : far nodes drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

const int windowWidth = 1024 * 2;
const int windowHeight = 768 * 2;
const int MaxNodes = 100000;
Node NodeContainer[MaxNodes];

void SortNodes() { std::sort(&NodeContainer[0], &NodeContainer[MaxNodes]); }

int main(void) {
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

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Allow the capture of the escape key
    glfwSetInputMode(window, GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED); // Hide mouse and enable unlimited movement

    glfwPollEvents();
    glfwSetCursorPos(window, windowWidth / 2.0f, windowHeight / 2.0f);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("nodeVertexShader.glsl", "nodeFragmentShader.glsl");

    // Vertex shader
    GLuint CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
    GLuint CameraUp_worldspace_ID = glGetUniformLocation(programID, "CameraUp_worldspace");
    GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    // fragment shader
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    static GLfloat *g_particule_position_size_data = new GLfloat[MaxNodes * 4];
    static GLubyte *g_particule_color_data = new GLubyte[MaxNodes * 4];

    for (int i = 0; i < MaxNodes; i++) {
        NodeContainer[i].cameradistance = -1.0f;
    }

    GLuint Texture = loadDDS("sphere512.DDS");

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxNodes * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxNodes * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    for (int i = 0; i < MaxNodes; i++) {
        NodeContainer[i].pos =
            glm::vec3((rand() % 200 - 100.0f), (rand() % 200 - 100.0f), (rand() % 200 - 100.0f));

        float spread = 15.0f;
        glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
        // Very bad way to generate a random direction;
        // See for instance
        // http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution
        // instead, combined with some user-controlled parameters (main direction, spread, etc)
        glm::vec3 randomdir =
            glm::vec3((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f,
                      (rand() % 2000 - 1000.0f) / 1000.0f);

        NodeContainer[i].speed = maindir + randomdir * spread;

        // Very bad way to generate a random color
        NodeContainer[i].r = rand() % 256;
        NodeContainer[i].g = rand() % 256;
        NodeContainer[i].b = rand() % 256;
        NodeContainer[i].a = 255;

        NodeContainer[i].size = (rand() % 1000) / 2000.0f + 0.1f;
    }

    double lastTime = glfwGetTime();

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
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;

        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp,
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

        // Generate 10 new particle each millisecond,
        // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
        // new particles will be huge and the next frame even longer.
        int newparticles = (int)(delta * 10000.0);
        if (newparticles > (int)(0.016f * 10000.0))
            newparticles = (int)(0.016f * 10000.0);

        // Simulate all particles
        int NodeCount = 0;
        for (int i = 0; i < MaxNodes; i++) {

            Node &p = NodeContainer[i]; // shortcut

            // Simulate simple physics : gravity only, no collisions
            // p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
            // p.pos += p.speed * (float)delta;
            p.cameradistance = glm::length2(p.pos - CameraPosition);
            // NodeContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

            // Fill the GPU buffer
            g_particule_position_size_data[4 * NodeCount + 0] = p.pos.x;
            g_particule_position_size_data[4 * NodeCount + 1] = p.pos.y;
            g_particule_position_size_data[4 * NodeCount + 2] = p.pos.z;

            g_particule_position_size_data[4 * NodeCount + 3] = p.size;

            g_particule_color_data[4 * NodeCount + 0] = p.r;
            g_particule_color_data[4 * NodeCount + 1] = p.g;
            g_particule_color_data[4 * NodeCount + 2] = p.b;
            g_particule_color_data[4 * NodeCount + 3] = p.a;

            NodeCount++;
        }

        SortNodes();

        // printf("%d ",NodeCount);

        // Update the buffers that OpenGL uses for rendering.
        // There are much more sophisticated means to stream data from the CPU to the GPU,
        // but this is outside the scope of this tutorial.
        // http://www.opengl.org/wiki/Buffer_Object_Streaming

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxNodes * 4 * sizeof(GLfloat), NULL,
                     GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
                                      // See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, NodeCount * sizeof(GLfloat) * 4,
                        g_particule_position_size_data);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxNodes * 4 * sizeof(GLubyte), NULL,
                     GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
                                      // See above link for detai#extension
                                      // GL_ARB_explicit_uniform_location : requirels.
        glBufferSubData(GL_ARRAY_BUFFER, 0, NodeCount * sizeof(GLubyte) * 4,
                        g_particule_color_data);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0],
                    ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
            0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );

        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            1, // attribute. No particular reason for 1, but must match the layout in the shader.
            4, // size : x + y + z + size => 4
            GL_FLOAT, // type
            GL_FALSE, // normalized?#extension GL_ARB_explicit_uniform_location : require
            0,        // stride
            (void *)0 // array buffer offset
        );

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
            2, // attribute. No particular reason for 1, but must match the layout in the shader.
            4, // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE, // type
            GL_TRUE,  // normalized?    *** YES, this means that the unsigned char[4] will be
                      // accessible with a vec4 (floats) in the shader ***
            0,        // stride
            (void *)0 // array buffer offset
        );

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when
        // rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

        // Draw the particles !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in NodeCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NodeCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    delete[] g_particule_position_size_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();

    return 0;
}
