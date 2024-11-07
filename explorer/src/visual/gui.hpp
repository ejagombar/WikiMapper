#include <glad/glad.h>
#include <glm/trigonometric.hpp>

#include "../../lib/camera.hpp"
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

#include "../../lib/texture.hpp"

class gui {
  public:
    gui() { init(); };
    ~gui() {};

  private:
    int init();
    void loop();

    const unsigned int SCR_WIDTH = 1920;
    const unsigned int SCR_HEIGHT = 1080;

    float lastX;
    float lastY;
    bool firstMouse = true;

    static void framebuffer_size_callback_static(GLFWwindow *window, int width, int height);
    static void mouse_callback_static(GLFWwindow *window, double xpos, double ypos);
    static void scroll_callback_static(GLFWwindow *window, double xoffset, double yoffset);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void processInput(GLFWwindow *window);

    Camera camera;

    void print(std::string str) { std::cout << str << std::endl; }

    // timing
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f;
};
