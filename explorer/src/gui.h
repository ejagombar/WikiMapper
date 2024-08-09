#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include "../lib/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

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

class gui {
  public:
    gui(const int &MaxNodes);
    ~gui(){};
    int init();

  private:
    void generateNodeData(Node *NodeContainer, int size);
    void loop();
    void sortNodes();
    int initWindow();

    int m_MaxNodes;
    double lastTime;
    const int windowWidth = 1024 * 2;
    const int windowHeight = 768 * 2;

    Camera camera;
    GLFWwindow *window;

    GLuint billboard_vertex_buffer;
    GLuint node_position_buffer;
    GLuint node_color_buffer;
    GLuint programID;
    GLuint shaderProgram;
    GLuint Texture;
    GLuint TextureID;
    GLuint CameraRight_worldspace_ID;
    GLuint CameraUp_worldspace_ID;
    GLuint ViewProjMatrixID;

    GLuint VAO, VBO;

    Node *NodeContainer;

    std::vector<glm::vec3> lineVertices;
    GLfloat *g_node_position_size_data;
    GLubyte *g_node_color_data;
};

#endif // GUI_H
