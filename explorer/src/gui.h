#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

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

template <unsigned int MaxNodes> class gui {
  public:
    gui();
    ~gui();
    int init();

  private:
    void generateNodeData(Node *NodeContainer);
    void loop();
    void SortNodes();
    int initWindow();

    const int windowWidth = 1024 * 2;
    const int windowHeight = 768 * 2;
    GLFWwindow *window;
    Node NodeContainer[MaxNodes];
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    GLuint programID;
    GLuint Texture;
    GLuint TextureID;
    GLuint CameraRight_worldspace_ID;
    GLuint CameraUp_worldspace_ID;
    GLuint ViewProjMatrixID;
    double lastTime;
    static GLfloat *g_particule_position_size_data;
    static GLubyte *g_particule_color_data;
};

#endif // GUI_H
